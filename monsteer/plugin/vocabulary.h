
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

#ifndef MONSTEER_PLUGIN_VOCABULARY_H
#define MONSTEER_PLUGIN_VOCABULARY_H

#include "plugin/spikes_zeq_generated.h"

#include <zeq/types.h>
#include <zeq/event.h>

#include <map>

namespace monsteer
{
namespace plugin
{

static const zeq::uint128_t EVENT_EOS(
    zeq::make_uint128( "monsteer::streaming::EndOfStream" ));

typedef std::multimap< float, uint32_t > SpikeMap;

zeq::Event serializeSpikes( const SpikeMap& spikes );
SpikeMap deserializeSpikes( const zeq::Event& event );

}
}
#endif
