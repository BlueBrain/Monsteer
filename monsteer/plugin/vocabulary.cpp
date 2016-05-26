
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

#include "vocabulary.h"

#include "plugin/spikes_generated.h"
#include "plugin/endOfStream_generated.h"

namespace monsteer
{
namespace plugin
{

zeroeq::FBEvent serializeEOS()
{
    zeroeq::FBEvent event( EVENT_ENDOFSTREAM, ::zeroeq::EventFunc( ));
    flatbuffers::FlatBufferBuilder& fbb = event.getFBB();
    EndOfStreamBuilder builder( fbb );
    builder.add_endofstream( true );
    fbb.Finish( builder.Finish( ));
    return event;
}

zeroeq::FBEvent serializeSpikes( const SpikeMap& spikes )
{
    zeroeq::FBEvent event( EVENT_SPIKES, ::zeroeq::EventFunc( ));

    std::vector< Spike > spikeVector;
    spikeVector.reserve( spikes.size( ));
    for( std::multimap< float, uint32_t >::const_iterator i = spikes.begin();
         i != spikes.end(); ++i )
    {
        spikeVector.push_back( Spike( i->first, i->second ));
    }

    flatbuffers::FlatBufferBuilder& fbb = event.getFBB();
    auto vector = fbb.CreateVectorOfStructs( spikeVector.data(),
                                             spikeVector.size() );
    fbb.Finish( CreateSpikes( fbb, vector ));
    return event;
}

SpikeMap deserializeSpikes( const zeroeq::FBEvent& event )
{
    auto data = GetSpikes( event.getData( ));
    SpikeMap spikes;
    for( flatbuffers::uoffset_t i = 0; i < data->spikes()->Length(); ++i )
    {
        const Spike* spike = data->spikes()->Get( i );
        spikes.insert( std::make_pair( spike->time(), spike->cell( )));
    }
    return spikes;
}

bool deserializeEOS( const zeroeq::FBEvent&  )
{
    return true;
}

}
}
