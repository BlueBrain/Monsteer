
/* Copyright (c) 2006-2015, Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
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


#ifndef NRP_STEERING_VOCABULARY_H
#define NRP_STEERING_VOCABULARY_H

#include <zeq/types.h>
#include <zeq/event.h>

#include <nrp/runSimTrigger_zeq_generated.h>
#include <nrp/statusRequestMsg_zeq_generated.h>
#include <nrp/proxyStatusMsg_zeq_generated.h>

#include <monsteer/types.h>

namespace monsteer
{
namespace steering
{


struct SimulationRunTrigger
{
    SimulationRunTrigger()
        : duration(20.0)
    {}

    std::string messageID;
    double duration;

};

struct ProxyStatus 
{
    enum State
    {
        READY = 0u,
        BUSY = 1u
    };

    ProxyStatus()
    {}

    std::string messageID;
    State state;
};

struct StatusRequest
{
    StatusRequest()
    {}

    std::string messageID;
};

zeq::Event serializeSimulationRunTrigger( const std::string& messageID,
                                       const double duration );

SimulationRunTrigger deserializeSimulationRunTrigger( const zeq::Event& event );

zeq::Event serializeStatusRequest( const std::string& messageID );

StatusRequest deserializeStatusRequest( const zeq::Event& event );

zeq::Event serializeProxyStatus( const std::string& messageID,
                                 const ProxyStatus::State state );

ProxyStatus deserializeProxyStatus( const zeq::Event& event );


}
}
#endif // NRP_STEERING_VOCABULARY_H
