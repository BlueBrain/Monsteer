
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

#ifndef NRP_PLUGIN_NESTSIMULATOR_H
#define NRP_PLUGIN_NESTSIMULATOR_H

#include <nrp/steering/simulatorPlugin.h>
#include <lunchbox/uri.h>
#include <boost/scoped_ptr.hpp>
#include <nrp/steering/vocabulary.h>

namespace monsteer
{
namespace steering
{

/** Steering interface to NEST simulations */
class NESTSimulator : public monsteer::SimulatorPlugin
{
public:
    explicit NESTSimulator( const SimulatorPluginInitData& pluginData );

    /** Check if this plugin can handle the given plugin data. */
    static bool handles( const SimulatorPluginInitData& pluginData );

    /** @copydoc monsteer::Simulator::injectStimulus */
    void injectStimulus( const std::string& jsonParameters,
                         const brion::uint32_ts& cells ) final;

    /** @copydoc monsteer::Simulator::injectMultipleStimuli */
    void injectMultipleStimuli( const std::string& jsonParameters,
                                const brion::uint32_ts& cells ) final;

    /** @copydoc monsteer::Simulator::play */
    void play() final;

    /** @copydoc monsteer::Simulator::pause */
    void pause() final;

    /** @copydoc monsteer::Simulator::simulate*/
    void simulate( const double duration ) final;



private:
    boost::scoped_ptr< zeq::Subscriber > _replySubscriber;
    boost::scoped_ptr< zeq::Publisher > _requestPublisher;
    monsteer::steering::ProxyStatus::State _proxyState;
    uint32_t _lastReceivedStatusID = 0;
    uint32_t _lastAcknowledgeID = 0;

    /** @copydoc monsteer::Simulator::_onProxyStatusUpdate */
    void _onProxyStatusUpdate( const zeq::Event& event );

    /** @copydoc monsteer::Simulator::barrier */

    void barrier();
};

}
}
#endif

