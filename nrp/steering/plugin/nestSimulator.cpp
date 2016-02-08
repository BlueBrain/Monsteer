
/* Copyright (c) 2006-2015, Juan Hernando <jhernando@fi.upm.es>
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

#include "nestSimulator.h"

#include <monsteer/types.h>
#include <nrp/steering/vocabulary.h>
#include <monsteer/steering/vocabulary.h>

#include <zeq/subscriber.h>
#include <zeq/publisher.h>
#include <zeq/uri.h>

#include <lunchbox/debug.h>
#include <lunchbox/pluginRegisterer.h>

#include <iostream>
#include <chrono>

namespace monsteer
{
namespace steering
{

namespace
{
lunchbox::PluginRegisterer< NESTSimulator > registerer;
}

NESTSimulator::NESTSimulator( const SimulatorPluginInitData& pluginData )
    : _replySubscriber( new zeq::Subscriber( zeq::URI("nrp-downstream") ))
    , _requestPublisher( new zeq::Publisher(  zeq::URI("nrp-upstream" )) )
                                            
{
      _replySubscriber->registerHandler(EVENT_PROXYSTATUSMSG, boost::bind( &NESTSimulator::_onProxyStatusUpdate, this, _1 )); 
}

bool NESTSimulator::handles( const SimulatorPluginInitData& pluginData )
{
    return pluginData.subscriber.getScheme() ==
                      MONSTEER_NEST_SIMULATOR_PLUGIN_SCHEME;
}

void NESTSimulator::barrier()
{
    uint32_t attempt = 1;
    const uint32_t timeout = 1000;
    const uint32_t max_attempts = 20;
    uint32_t requestMsgID = 0;

    auto startTime = std::chrono::high_resolution_clock::now();

    while (_proxyState != monsteer::steering::ProxyStatus::State::READY)
    //_replySubscriber->receive(0);
    {
        // Get messages
        while(_replySubscriber->receive(0));

        if ( attempt > max_attempts )
            throw std::runtime_error("Monsteer-steering had a timeout.");

        // Get Time
        const auto endTime = std::chrono::high_resolution_clock::now();
        const uint32_t elapsed =
            std::chrono::nanoseconds( endTime - startTime ).count() /
            1000000;

        if( elapsed > timeout )
        {
            if(_lastReceivedStatusID <= requestMsgID )
            {
                attempt ++;
            }
            requestMsgID = _lastReceivedStatusID;
            _requestPublisher->publish(
                    serializeStatusRequest( std::to_string(++requestMsgID)));
            startTime = std::chrono::high_resolution_clock::now();
        }
    }
}

void NESTSimulator::injectStimulus( const std::string& jsonParameters,
                                    const brion::uint32_ts& cells )
{
    // The messageID is irrelevant for the moment
    _requestPublisher->publish(
        serializeStimulus( "", cells, jsonParameters, /*single*/ false ));
}

void NESTSimulator::injectMultipleStimuli( const std::string& jsonParameters,
                                           const brion::uint32_ts& cells )
{
    // The messageID is irrelevant for the moment
    _requestPublisher->publish(
        serializeStimulus( "", cells, jsonParameters, /*multiple*/ true ));
}

void NESTSimulator::play()
{
}


void NESTSimulator::pause()
{
}

void NESTSimulator::simulate( const double duration)
{
    //_requestPublisher->publish(
     //   serializePlaybackState( "", SimulationPlaybackState::ONDEMAND ));
    //_replySubscriber->receive(0);
    
    barrier();
    _proxyState = monsteer::steering::ProxyStatus::State::BUSY;
    _requestPublisher->publish(
            serializeSimulationRunTrigger( "", duration ));
    barrier();
}


void NESTSimulator::_onProxyStatusUpdate( const zeq::Event& event )  
{
    LBASSERT( event.getType() == EVENT_PROXYSTATUSMSG ); 
    const monsteer::steering::ProxyStatus& state = 
        monsteer::steering::deserializeProxyStatus( event );
    _proxyState = state.state;
    _lastReceivedStatusID = std::stoi( state.messageID );
}


}
}
