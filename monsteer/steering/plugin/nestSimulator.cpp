
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
#include <zeq/uri.h>

#include <lunchbox/debug.h>
#include <lunchbox/pluginRegisterer.h>

namespace monsteer
{
namespace steering
{

namespace
{
const std::string nestSimulatorScheme = "nestSimulator";

lunchbox::PluginRegisterer< NESTSimulator > registerer;
}

NESTSimulator::NESTSimulator( const SimulatorPluginInitData& pluginData )
    : _replySubscriber( new zeq::Subscriber( zeq::URI( pluginData.subscriber ),
                                             zeq::DEFAULT_SESSION ))
    , _requestPublisher( new zeq::Publisher( ))
{
}

bool NESTSimulator::handles( const SimulatorPluginInitData& pluginData )
{
    return pluginData.subscriber.getScheme() == nestSimulatorScheme;
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
