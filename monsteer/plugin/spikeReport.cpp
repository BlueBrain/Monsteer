
/* Copyright (c) 2006-2015, Juan Hernando <jhernando@fi.upm.es>
 *                          Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *
 * This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "spikeReport.h"

#include <zeroeq/zeroeq.h>

#include <lunchbox/clock.h>
#include <lunchbox/pluginRegisterer.h>
#include <lunchbox/uri.h>

#include <brion/version.h>


extern "C" int LunchboxPluginGetVersion() { return BRION_VERSION_ABI; }
extern "C" bool LunchboxPluginRegister()
{
    lunchbox::PluginRegisterer< monsteer::plugin::SpikeReport > registerer;
    return true;
}

namespace monsteer { namespace plugin {


URI toHostAndPort( const URI& uri )
{
    URI out;
    out.setHost(uri.getHost());
    out.setPort(uri.getPort());
    return out;
}

SpikeReport::SpikeReport( const SpikeReportInitData&  initData) :
    brion::SpikeReportPlugin(initData)
{
    _uri = toHostAndPort(initData.getURI());
    switch (getAccessMode()) {
    case brion::MODE_READ:
    {
        _subscriber.reset(
                    new zeroeq::Subscriber(
                        zeroeq::URI(_uri),
                        zeroeq::DEFAULT_SESSION
                        )
                    );

        _subscriber->subscribe(
                    SpikesEvent::ZEROBUF_TYPE_IDENTIFIER(),
                    [&](const void* data,const size_t size)
        {
            _onSpikes(SpikesEvent::create(data,size));
        }
        );

        _subscriber->subscribe(
                    EndOfStream::ZEROBUF_TYPE_IDENTIFIER(),
                    [&] { _onEOS(); }
        );

        _subscriber->subscribe(
                    SeekForwardEvent::ZEROBUF_TYPE_IDENTIFIER(),
                    [&](const void* data,const size_t size)
        {
            _onSeekForward(SeekForwardEvent::create(data,size));
        }
        );
        break;
    }
    case brion::MODE_WRITE:
    {
        _publisher.reset(new zeroeq::Publisher(zeroeq::URI(_uri)));
        break;
    }
    default:
        break;
    }
}


bool SpikeReport::handles( const SpikeReportInitData&  pluginData)
{
    return pluginData.getURI().getScheme() == MONSTEER_BRION_SPIKES_PLUGIN_SCHEME;
}


const lunchbox::URI& SpikeReport::getURI() const
{
    if( _publisher )
        return _publisher->getURI().toServusURI();

    return _uri;
}

void SpikeReport::close()
{
    if( _publisher )
    {
        _publisher->publish( EndOfStream::ZEROBUF_TYPE_IDENTIFIER());
    }
    _state = State::ENDED;

}

std::vector<brion::Spike> SpikeReport::read(float min)
{
    std::vector<brion::Spike> spikes;

    while( _state == State::OK &&
           !_publisherFinished &&
           _publisherTimeStamp < min )
    {
        _subscriber->receive();
    }

    if( _state == State::FAILED )
    {
        return spikes;
    }

    if( _publisherFinished && _publisherTimeStamp < min )
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ENDED;
        spikes =  std::move(_spikes);
        return spikes;
    }

    size_t index = 0;
    for( ;index < _spikes.size() ;++index )
    {
        if( _spikes[index].first < min )
        {
            spikes.push_back(_spikes[index]);
        }
        else
        {
            _currentTime = _spikes[index].first ;
            break;
        }
    }

    _spikes.erase(_spikes.begin(),_spikes.begin()+index);

    return spikes;
}

std::vector<brion::Spike> SpikeReport::readUntil(float max)
{
    std::vector<brion::Spike> spikes;

    while( _state == State::OK &&
           !_publisherFinished &&
           _publisherTimeStamp < max )
    {
        _subscriber->receive();
    }

    if( _publisherFinished && _publisherTimeStamp != max )
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ENDED;
        spikes = std::move(_spikes);
        return spikes;
    }

    size_t index = 0;
    for( ;index < _spikes.size() ;++index )
    {
        if( _spikes[index].first == max )
        {
            _currentTime = max;
            break;
        }
        spikes.push_back(_spikes[index]);
    }

    _spikes.erase(_spikes.begin(),_spikes.begin()+index);

    return spikes;
}

void  SpikeReport::readSeek(float toTimeStamp)
{
    if( toTimeStamp < _currentTime )
    {
        throw std::runtime_error("Bakward seek is not supported");
    }

    while( _state == State::OK &&
           !_publisherFinished &&
           _publisherTimeStamp < toTimeStamp )
    {
        _subscriber->receive();
    }


    if( _state == State::FAILED )
    {
        return ;
    }

    if( _publisherFinished && _currentTime < toTimeStamp )
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ENDED;
        _spikes.clear();
        return ;
    }

    auto position = std::upper_bound(
                _spikes.begin(),_spikes.end(),
                toTimeStamp,
                [](float val,const brion::Spike & spike){
                    return spike.first >= val;
                }
    );

    _spikes.erase(_spikes.begin(),position);
    _currentTime = toTimeStamp;
}

void  SpikeReport::writeSeek(float toTimeStamp)
{
    if( toTimeStamp < _currentTime )
    {
        throw std::runtime_error("Backward seek is not supported");
    }

    SeekForwardEvent event;
    event.setTime(toTimeStamp);
    _publisher->publish(event);
    _currentTime = toTimeStamp;
}

void SpikeReport::write(const std::vector<brion::Spike>& spikes)
{
    if( !spikes.size() )
    {
        return;
    }

    SpikesEvent event;

    SpikesEvent::Spikes& data = event.getSpikes();
    for( const auto& spike : spikes )
    {
        data.push_back({spike.first, spike.second});
    }

    _publisher->publish(event);

    _currentTime = spikes.rbegin()->first + std::numeric_limits<float>::epsilon();
}

void SpikeReport::_onEOS()
{
    _publisherFinished = true;
}

void SpikeReport::_onSeekForward(ConstSeekForwardEventPtr event)
{
    _publisherTimeStamp = event->getTime();
}

void SpikeReport::_onSpikes(ConstSpikesEventPtr event)
{
    const SpikesEvent::Spikes & spikes = event->getSpikes();
    auto size = spikes.size();

    if( !size )
    {
        return;
    }

    for( size_t i = 0; i < size ; ++i )
    {
        const Spike& spike = spikes[i];
        pushBackSpike({spike.getTime(),spike.getCell()},_spikes);
    }
    _publisherTimeStamp = spikes[size-1].getTime();

}


}} // namespaces
