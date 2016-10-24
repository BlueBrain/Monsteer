
/* Copyright (c) 2006-2016, Juan Hernando <jhernando@fi.upm.es>
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

#include "simulator.h"
#include "simulatorPlugin.h"

#include <lunchbox/plugin.h>
#include <lunchbox/pluginFactory.h>

namespace monsteer
{

class Simulator::Impl
{
public:
    typedef lunchbox::PluginFactory< SimulatorPlugin > SimulatorPluginFactory;

    explicit Impl( const SimulatorPluginInitData& initData )
        : plugin( SimulatorPluginFactory::getInstance().create( initData ))
    {
    }

    std::unique_ptr< SimulatorPlugin > plugin;
};

Simulator::Simulator( const URI& uri )
    // The publisher URI is the scheme part of the subscriber URI for the
    // moment.
    : _impl( new Simulator::Impl( SimulatorPluginInitData( uri )))
{
}

Simulator::~Simulator()
{
    delete _impl;
}

void Simulator::injectStimulus( const std::string& jsonParameters,
                                const brion::uint32_ts& cells )
{
    _impl->plugin->injectStimulus( jsonParameters, cells );
}

void Simulator::injectMultipleStimuli( const std::string& jsonParameters,
                                       const brion::uint32_ts& cells )
{
    _impl->plugin->injectMultipleStimuli( jsonParameters, cells );
}

void Simulator::play()
{
    _impl->plugin->play();
}

void Simulator::pause()
{
    _impl->plugin->pause();
}

}
