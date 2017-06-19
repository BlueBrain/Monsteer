/* Copyright (c) 2006-2017, Juan Hernando <jhernando@fi.upm.es>
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

#include <lunchbox/clock.h>
#include <lunchbox/pluginRegisterer.h>
#include <lunchbox/uri.h>

#include <brion/version.h>

#include <zeroeq/zeroeq.h>

#include <cmath>

extern "C" int LunchboxPluginGetVersion()
{
    return BRION_VERSION_ABI;
}
extern "C" bool LunchboxPluginRegister()
{
    lunchbox::PluginRegisterer<monsteer::plugin::SpikeReport> registerer;
    return true;
}

#define RECEIVE_TIMEOUT 100 // ms

namespace monsteer
{
namespace plugin
{
zeroeq::URI toHostAndPort(const URI& uri)
{
    zeroeq::URI out;
    out.setHost(uri.getHost());
    out.setPort(uri.getPort());
    return out;
}

SpikeReport::SpikeReport(const SpikeReportInitData& initData)
    : brion::SpikeReportPlugin(initData)
{
    const auto uri = toHostAndPort(initData.getURI());
    switch (getAccessMode())
    {
    case brion::MODE_READ:
    {
        if (uri.getHost().empty() || uri.getPort() == 0)
            _subscriber.reset(new zeroeq::Subscriber);
        else
            _subscriber.reset(new zeroeq::Subscriber(uri));

        _subscriber->subscribe(SpikesEvent::ZEROBUF_TYPE_IDENTIFIER(),
                               [&](const void* data, const size_t size) {
                                   _onSpikes(SpikesEvent::create(data, size));
                               });

        _subscriber->subscribe(EndOfStream::ZEROBUF_TYPE_IDENTIFIER(),
                               [&] { _onEOS(); });

        _subscriber->subscribe(SeekForwardEvent::ZEROBUF_TYPE_IDENTIFIER(),
                               [&](const void* data, const size_t size) {
                                   _onSeekForward(
                                       SeekForwardEvent::create(data, size));
                               });
        break;
    }
    case brion::MODE_WRITE:
    {
        _publisher.reset(new zeroeq::Publisher(uri));
        _uri = _publisher->getURI().toServusURI();
        _uri.setScheme(MONSTEER_BRION_SPIKES_PLUGIN_SCHEME);
        break;
    }
    default:
        break;
    }
}

bool SpikeReport::handles(const SpikeReportInitData& pluginData)
{
    return pluginData.getURI().getScheme() ==
           MONSTEER_BRION_SPIKES_PLUGIN_SCHEME;
}

std::string SpikeReport::getDescription()
{
    return std::string("ZeroEQ streaming spike report: ") +
           MONSTEER_BRION_SPIKES_PLUGIN_SCHEME + "://";
}

void SpikeReport::close()
{
    if (_publisher)
        _publisher->publish(EndOfStream::ZEROBUF_TYPE_IDENTIFIER());

    _state = State::ended;
}

brion::Spikes SpikeReport::read(float min)
{
    brion::Spikes spikes;

    // First empty the buffered data with no timeout no matter the timestamps
    // received. In this case this gives the opportunity to update the end
    // time without having to use a minimum timestamp that may cause the client
    // to block.
    while (_subscriber->receive(0))
        ;

    while (_state == State::ok && !_publisherFinished &&
           _publisherTimeStamp < min)
    {
        _subscriber->receive(RECEIVE_TIMEOUT);
        checkNotInterrupted();
    }

    if (_state == State::failed)
        return spikes;

    if (_publisherFinished)
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ended;
    }
    else
    {
        // current time should be > last read spike
        // The writer guarantees that no spikes are emitted with timestamp
        // <= last emited spike timestamp
        _currentTime = std::nextafter(_publisherTimeStamp,
                                      std::numeric_limits<float>::max());
    }

    spikes = std::move(_spikes);
    _endTime = std::max(_endTime, _publisherTimeStamp);

    return spikes;
}

brion::Spikes SpikeReport::readUntil(float toTimeStamp)
{
    brion::Spikes spikes;

    // Empty the receive buffer without blocking
    while (_subscriber->receive(0))
        ;

    while (_state == State::ok && !_publisherFinished &&
           _publisherTimeStamp < toTimeStamp)
    {
        _subscriber->receive(RECEIVE_TIMEOUT);
        checkNotInterrupted();
    }

    if (_publisherFinished)
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ended;
        spikes = std::move(_spikes);
        return spikes;
    }

    auto pos = std::lower_bound(_spikes.begin(), _spikes.end(), toTimeStamp,
                                [](const brion::Spike& spike, float val) {
                                    return spike.first < val;
                                });

    std::for_each(_spikes.begin(), pos,
                  [&spikes, this](const brion::Spike& spike) {
                      pushBack(spike, spikes);
                  });

    _spikes.erase(_spikes.begin(), pos);
    _currentTime = _publisherTimeStamp;
    _endTime = _publisherTimeStamp;

    return spikes;
}

void SpikeReport::readSeek(float toTimeStamp)
{
    if (toTimeStamp < _currentTime)
        throw std::runtime_error("Backward seek is not supported");

    // Empty the receive buffer without blocking
    while (_subscriber->receive(0))
        ;

    while (_state == State::ok && !_publisherFinished &&
           _publisherTimeStamp < toTimeStamp)
    {
        _subscriber->receive(RECEIVE_TIMEOUT);
        checkNotInterrupted();
    }

    if (_state == State::failed)
        return;

    if (_publisherFinished && _currentTime < toTimeStamp)
    {
        _currentTime = brion::UNDEFINED_TIMESTAMP;
        _state = State::ended;
        _spikes.clear();
        return;
    }

    auto position =
        std::upper_bound(_spikes.begin(), _spikes.end(), toTimeStamp,
                         [](float val, const brion::Spike& spike) {
                             return spike.first >= val;
                         });

    _spikes.erase(_spikes.begin(), position);
    _currentTime = toTimeStamp;
    if (_spikes.empty())
        _endTime = toTimeStamp;
}

void SpikeReport::writeSeek(float toTimeStamp)
{
    if (toTimeStamp < _currentTime)
        throw std::runtime_error("Backward seek is not supported");

    SeekForwardEvent event;
    event.setTime(toTimeStamp);
    _publisher->publish(event);
    _currentTime = toTimeStamp;
}

void SpikeReport::write(const brion::Spike* spikes, const size_t size)
{
    if (size == 0)
        return;

    SpikesEvent event;

    SpikesEvent::Spikes& data = event.getSpikes();
    for (size_t i = 0; i != size; ++i)
        data.push_back({spikes[i].first, spikes[i].second});

    _publisher->publish(event);

    _currentTime =
        spikes[size - 1].first + std::numeric_limits<float>::epsilon();
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
    const SpikesEvent::Spikes& spikes = event->getSpikes();
    auto size = spikes.size();

    if (!size)
        return;

    for (size_t i = 0; i < size; ++i)
    {
        const Spike& spike = spikes[i];
        pushBack({spike.getTime(), spike.getCell()}, _spikes);
    }

    // This timestamp has to updated with the incoming spikes, not the filtered
    // ones.
    _publisherTimeStamp = spikes[size - 1].getTime();
}
}
} // namespaces
