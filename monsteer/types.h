/* Copyright (c) 2006-2016, Ahmet Bilgili <ahmet.bilgili@epfl.ch>
 *                          Juan Hernando <jhernando@fi.upm.es>
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

#ifndef MONSTEER_TYPES_H
#define MONSTEER_TYPES_H

#include <brion/types.h>
#include <lunchbox/log.h>
#include <lunchbox/types.h>

#define MONSTEER_BRION_SPIKES_PLUGIN_SCHEME    std::string( "monsteer" )
#define MONSTEER_NEST_SIMULATOR_PLUGIN_SCHEME  std::string( "nest" )

namespace zeroeq
{
class Subscriber;
class Publisher;
class Event;
}

/** @namespace monsteer MONSTEER types */
namespace monsteer
{
using brion::URI;

class Simulator;

typedef std::vector< std::string > Strings;

typedef boost::shared_ptr< Simulator > SimulatorPtr;
}

#endif
