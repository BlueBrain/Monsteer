
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


#ifndef STEERING_VOCABULARY_H
#define STEERING_VOCABULARY_H

#include <zeroeq/types.h>
#include <zeroeq/event.h>

#include <monsteer/playbackState_zeroeq_generated.h>
#include <monsteer/stimulus_zeroeq_generated.h>

#include <monsteer/types.h>

namespace monsteer
{
namespace steering
{

struct Stimulus
{
    Stimulus()
        : multiple(false)
    {}

    std::string messageID;
    brion::uint32_ts cells;
    std::string params;
    bool multiple;
};

struct SimulationPlaybackState
{
    enum State
    {
        PAUSE = 0u,
        PLAY = 1u
    };

    SimulationPlaybackState()
        : state( PLAY ) {}

    std::string messageID;
    State state;
};

zeroeq::Event serializeStimulus( const std::string& messageID,
                                 const brion::uint32_ts& cells,
                                 const std::string& params,
                                 const bool multiple );
zeroeq::Event serializeStimulus( const Stimulus& stimulus );

Stimulus deserializeStimulus( const zeroeq::Event& event );

zeroeq::Event serializePlaybackState(  const std::string& messageID,
                                   const SimulationPlaybackState::State state );

SimulationPlaybackState deserializePlaybackState( const zeroeq::Event& event );

}
}
#endif // STEERING_VOCABULARY_H
