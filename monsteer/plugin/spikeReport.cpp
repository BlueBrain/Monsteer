
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
#include "vocabulary.h"

#include <zeq/subscriber.h>
#include <zeq/publisher.h>
#include <zeq/vocabulary.h>
#include <zeq/event.h>
#include <zeq/uri.h>

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

namespace monsteer
{
namespace plugin
{

namespace
{
const uint32_t INTERNAL_TIMEOUT = 500;
}

URI toHostAndPort( const URI& uri )
{
    URI out;
    out.setHost( uri.getHost( ));
    out.setPort( uri.getPort( ));
    return out;
}

SpikeReport::SpikeReport( const brion::SpikeReportInitData& pluginData )
    : _uri( toHostAndPort( pluginData.getURI( )))
    , _lastEndTime( 0 )
    , _lastTimeStamp( -1 )
    , _closed( false )
{
    switch( pluginData.getAccessMode( ))
    {
    case brion::MODE_READ:
    {
        _subscriber.reset( new zeq::Subscriber( zeq::URI( _uri ),
                                                zeq::DEFAULT_SESSION ));
        _subscriber->registerHandler(
            EVENT_SPIKES,
            boost::bind( &SpikeReport::_onSpikes, this, _1 ));
        _subscriber->registerHandler(
            EVENT_EOS,
            boost::bind( &SpikeReport::_onEOS, this, _1 ));
        break;
    }
    case brion::MODE_WRITE:
    case brion::MODE_OVERWRITE:
    {
        _publisher.reset( new zeq::Publisher( zeq::URI( _uri )));
        break;
    }
    default:
         LBTHROW( std::runtime_error( "Access mode for Zeq streaming"
                                      "plugin is not implemented" ));
         break;
    }
}

bool SpikeReport::handles( const brion::SpikeReportInitData& pluginData )
{
    return pluginData.getURI().getScheme() == "monsteer";
}

const lunchbox::URI& SpikeReport::getURI() const
{
    if( _publisher )
        return _publisher->getURI();

    return _uri;
}

float SpikeReport::getStartTime() const
{
    if(  _spikes.empty( ))
        return brion::UNDEFINED_TIMESTAMP;
    return _spikes.begin()->first;
}

float SpikeReport::getEndTime() const
{
    if( _spikes.empty( ))
        return brion::UNDEFINED_TIMESTAMP;
    return _spikes.rbegin()->first;
}

void SpikeReport::writeSpikes( const brion::Spikes& spikes )
{
    const zeq::Event& event = serializeSpikes( spikes );
    _publisher->publish( event );
}

const brion::Spikes& SpikeReport::getSpikes() const
{
    return _spikes;
}

brion::SpikeReport::ReadMode SpikeReport::getReadMode() const
{
    return brion::SpikeReport::STREAM;
}

void SpikeReport::close()
{
    if( _publisher )
        _publisher->publish( zeq::Event( EVENT_EOS ));
    if( _subscriber )
        _closed = true; // _lastTimeStamp is not reused to avoid race conditions
}

void SpikeReport::_receiveBufferedMessages()
{
    if( _closed )
    {
        _lastTimeStamp = std::numeric_limits< float >::infinity();
        return;
    }

    if( _lastTimeStamp != std::numeric_limits< float >::infinity( ))
        while( _subscriber->receive( 0 ));
}

bool SpikeReport::waitUntil( const float timeStamp, const uint32_t timeout )
{
    _receiveBufferedMessages();

    // In order to fulfill the timeout strictly we need to use a clock because
    // _subscriber->receive returns in an indefinite amount of time if a
    // message is received.
    lunchbox::Clock timer;
    if( _lastTimeStamp <= timeStamp )
    {
        uint32_t elapsed = 0;
        while( elapsed < timeout )
        {
            if( _closed )
            {
                _lastTimeStamp = std::numeric_limits< float >::infinity();
                break;
            }

            while( _subscriber->receive( 0 ))
                ;
            if (_lastTimeStamp > timeStamp )
                break;

            _subscriber->receive(
                std::min( INTERNAL_TIMEOUT, timeout - elapsed ));
            if (_lastTimeStamp > timeStamp )
                break;
            elapsed = timer.getTime64();
        }
    }

    // Copying the spikes from _incoming to _spikes.
    const brion::Spikes::iterator last = _incoming.upper_bound( timeStamp );
    _spikes.insert( _incoming.begin(), last );

    if( !_spikes.empty( ) )
        _lastEndTime = _spikes.rbegin()->first;
    // And clearing the range [begin, last) from _incoming
    _incoming.erase( _incoming.begin(), last );

    // _lastTimestamp can contain +inf if the stream source has been closed.
    if( _lastTimeStamp == std::numeric_limits< float >::infinity( ))
        return !_incoming.empty();

    return _lastTimeStamp > timeStamp;
}

float SpikeReport::getNextSpikeTime()
{
    if( _incoming.empty( ))
        _receiveBufferedMessages();

    if( _incoming.empty( ))
    {
        if( _lastTimeStamp ==  std::numeric_limits< float >::infinity( ))
        {
            // The end of the stream has been reached and no spikes need to
            // be moved from incoming to the public container.
            return brion::UNDEFINED_TIMESTAMP;
        }
        // This works either for the case in which nothing has been read
        // yet and when incoming is empty and we have to return the spike
        // time that guarantees that waitUntil will make progress.
        return _lastEndTime;
    }
    return _incoming.begin()->first;
}

float SpikeReport::getLatestSpikeTime()
{
    _receiveBufferedMessages();

    if( _lastTimeStamp == -1 )
        return brion::UNDEFINED_TIMESTAMP;
    if( _lastTimeStamp ==  std::numeric_limits< float >::infinity( ))
    {
        const float timestamp = _incoming.empty() ?
                                _lastEndTime : _incoming.rbegin()->first;
        // Ensuring that the value returned by getLatestSpike makes
        // waitUntil digest all the spikes present in the buffer.
        return nextafterf(timestamp, INFINITY);
    }
    return _lastTimeStamp;
}

void SpikeReport::clear( const float startTime, const float endTime )
{
    if( endTime < startTime )
        return;
    _spikes.erase( _spikes.lower_bound( startTime ),
                   _spikes.upper_bound( endTime ));
}

void SpikeReport::_onSpikes( const zeq::Event& event )
{
    LBASSERT( event.getType() == EVENT_SPIKES );
    const SpikeMap& spikes = deserializeSpikes( event );
    _incoming.insert( spikes.begin(), spikes.end( ));
    if( !_incoming.empty() )
        _lastTimeStamp = _incoming.rbegin()->first;

}

void SpikeReport::_onEOS( const zeq::Event& event LB_UNUSED )
{
    LBASSERT( event.getType() == EVENT_EOS );
    _lastTimeStamp = std::numeric_limits< float >::infinity();
}

}
}
