
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
#include <monsteer/steering/vocabulary.h>

#include <zeq/subscriber.h>
#include <zeq/publisher.h>

#include <lunchbox/debug.h>
#include <lunchbox/pluginRegisterer.h>

namespace monsteer
{
namespace steering
{

namespace
{
lunchbox::PluginRegisterer< NESTSimulator > registerer;
}

NESTSimulator::NESTSimulator( const monsteer::SimulatorPluginInitData& pluginData )
{
    _replySubscriber.reset( new zeq::Subscriber( pluginData.subscriber ));
    _requestPublisher.reset( new zeq::Publisher( pluginData.publisher ));
}

bool NESTSimulator::handles( const monsteer::SimulatorPluginInitData& pluginData )
{
    const std::string url = "monsteer";
    return !pluginData.subscriber.getScheme().compare( 0, url.size(), url ) &&
           !pluginData.publisher.getScheme().compare( 0, url.size(), url );
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
    _requestPublisher->publish(
        serializePlaybackState( "", SimulationPlaybackState::PLAY ));
}


void NESTSimulator::pause()
{
    _requestPublisher->publish(
        serializePlaybackState( "", SimulationPlaybackState::PAUSE ));
}


}
}
