
/* Copyright (c) 2006-2017, Juan Hernando <jhernando@fi.upm.es>
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

#include <zeroeq/zeroeq.h>

#include <lunchbox/debug.h>
#include <lunchbox/pluginRegisterer.h>

#include <monsteer/steering/playbackState.h>
#include <monsteer/steering/stimulus.h>

namespace monsteer
{
namespace steering
{
namespace
{
lunchbox::PluginRegisterer<NESTSimulator> registerer;

using SubscriberPtr = std::unique_ptr<zeroeq::Subscriber>;
SubscriberPtr _createSubscriber(const SimulatorPluginInitData& pluginData)
{
    const zeroeq::URI uri(pluginData.subscriber);
    if (uri.getHost().empty() || uri.getPort() == 0)
        return SubscriberPtr(new zeroeq::Subscriber);
    return SubscriberPtr(new zeroeq::Subscriber(uri));
}
}

NESTSimulator::NESTSimulator(const SimulatorPluginInitData& pluginData)
    : _replySubscriber(_createSubscriber(pluginData))
    , _requestPublisher(new zeroeq::Publisher())
{
}

bool NESTSimulator::handles(const SimulatorPluginInitData& pluginData)
{
    return pluginData.subscriber.getScheme() ==
           MONSTEER_NEST_SIMULATOR_PLUGIN_SCHEME;
}

std::string NESTSimulator::getDescription()
{
    return std::string("NEST Simulator: ") +
           MONSTEER_NEST_SIMULATOR_PLUGIN_SCHEME + "://";
}

void NESTSimulator::injectStimulus(const std::string& jsonParameters,
                                   const brion::uint32_ts& cells)
{
    // The messageID is irrelevant for the moment
    _requestPublisher->publish(StimulusInjection("", "EVENT_STIMULUSINJECTION",
                                                 cells, jsonParameters,
                                                 /*single*/ false));
}

void NESTSimulator::injectMultipleStimuli(const std::string& jsonParameters,
                                          const brion::uint32_ts& cells)
{
    // The messageID is irrelevant for the moment
    _requestPublisher->publish(StimulusInjection("", "EVENT_STIMULUSINJECTION",
                                                 cells, jsonParameters,
                                                 /*multiple*/ true));
}

void NESTSimulator::play()
{
    _requestPublisher->publish(PlaybackState(State::PLAY));
}

void NESTSimulator::pause()
{
    _requestPublisher->publish(PlaybackState(State::PAUSE));
}
}
}
