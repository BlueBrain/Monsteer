
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

#include "vocabulary.h"

#include "stimulus_generated.h"
#include "playbackState_generated.h"

namespace monsteer
{
namespace steering
{
zeroeq::FBEvent serializeStimulus( const std::string& messageID,
                                 const brion::uint32_ts& cells,
                                 const std::string& params,
                                 const bool multiple )
{
    zeroeq::FBEvent event( EVENT_STIMULUSINJECTION, zeroeq::FBEventFunc( ));
    flatbuffers::FlatBufferBuilder& fbb = event.getFBB();

    // This is required to make FlatBuffers aware of the event fields that
    // contain their default values. Otherwise some information might be lost
    // e.g. 'multiple' parameter when it is set to false (0)
    fbb.ForceDefaults(true);

    auto fbMessageID = fbb.CreateString( messageID );
    auto fbEventType = fbb.CreateString( "EVENT_STIMULUSINJECTION" );
    auto fbCells = fbb.CreateVector( cells.data(), cells.size( ));
    auto fbParams = fbb.CreateString( params );

    fbb.Finish( CreateStimulusInjection( fbb, fbMessageID, fbEventType,
                                         fbCells, fbParams, multiple ));
    return event;
}

zeroeq::FBEvent serializeStimulus( const Stimulus& stimulus )
{
    return serializeStimulus( stimulus.messageID,
                              stimulus.cells,
                              stimulus.params,
                              stimulus.multiple );
}

Stimulus deserializeStimulus( const zeroeq::FBEvent& event )
{
    auto data = GetStimulusInjection( event.getData( ));

    Stimulus stimulus;
    stimulus.messageID = data->messageID()->c_str();
    stimulus.cells.reserve( data->cells()->Length( ));
    for( flatbuffers::uoffset_t i = 0; i < data->cells()->Length(); ++i )
    {
        stimulus.cells.push_back( data->cells()->Get( i ));
    }
    stimulus.params = data->params()->c_str();
    stimulus.multiple = data->multiple();

    return stimulus;
}

zeroeq::FBEvent serializePlaybackState( const std::string& messageID,
                                    const SimulationPlaybackState::State state )
{
    zeroeq::FBEvent event( EVENT_PLAYBACKSTATE, zeroeq::FBEventFunc( ));
    flatbuffers::FlatBufferBuilder& fbb = event.getFBB();
    auto fbMessageID = fbb.CreateString( messageID );
    fbb.Finish( CreatePlaybackState( fbb, fbMessageID, state ));
    return event;
}

SimulationPlaybackState deserializePlaybackState( const zeroeq::FBEvent& event )
{
    auto data = GetPlaybackState( event.getData( ));

    SimulationPlaybackState state;
    state.messageID = data->messageID()->c_str();
    state.state = (SimulationPlaybackState::State)data->state();

    return state;
}

}
}
