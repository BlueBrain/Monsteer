
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
#ifndef MONSTEER_SIMULATORPLUGIN_H
#define MONSTEER_SIMULATORPLUGIN_H

#include <monsteer/types.h>

#include <boost/noncopyable.hpp>

namespace monsteer
{

/** Init data for SimulatorPlugin
 * @version 0.4
 */
class SimulatorPluginInitData
{
public:
    explicit SimulatorPluginInitData( const URI& subscriber_ )
        : subscriber( subscriber_ )
    {}

    const URI subscriber;
};

/** Base interface for simulator steering plugins
 *
 * @version 0.2
 */
class SimulatorPlugin : public boost::noncopyable
{
public:
    /** @internal Needed by the PluginRegisterer. */
    typedef SimulatorPlugin InterfaceT;

    /** @internal Needed by the PluginRegisterer. */
    typedef SimulatorPluginInitData InitDataT;

    virtual ~SimulatorPlugin() {}

    /** @copydoc Simulator::injectStimulus */
    virtual void injectStimulus( const std::string& jsonParameters,
                                 const brion::uint32_ts& cells ) = 0;

    /** @copydoc Simulator::injectMultipleStimuli */
    virtual void injectMultipleStimuli( const std::string& jsonParameters,
                                        const brion::uint32_ts& cells ) = 0;

    /** @copydoc Simulator::play */
    virtual void play() = 0;

    /** @copydoc Simulator::pause */
    virtual void pause() = 0;
};

}

namespace std
{
inline string to_string( const monsteer::SimulatorPluginInitData& data )
{
    return to_string( data.subscriber );
}
}

#endif
