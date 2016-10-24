
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

#include <monsteer/types.h>
#include <monsteer/steering/simulator.h>
#include <monsteer/steering/simulatorPlugin.h>
#include <lunchbox/pluginRegisterer.h>

#define BOOST_TEST_MODULE Simulator
#include <boost/test/unit_test.hpp>

struct SimulatorData
{
    enum PlaybackState
    {
        PAUSE = 0u,
        PLAY = 1u
    };

    monsteer::URI subscriber;
    brion::uint32_ts cells;
    std::string stimulusString;
    PlaybackState playbackState;
};

SimulatorData globalData;

class DummySimulator : public monsteer::SimulatorPlugin
{
public:
    explicit DummySimulator( const monsteer::SimulatorPluginInitData& pluginData )
    {
        globalData.subscriber = pluginData.subscriber;
    }

    static bool handles( const monsteer::SimulatorPluginInitData& pluginData )
    {
        return pluginData.subscriber.getScheme() == "dummy";
    }

    void injectStimulus( const std::string& jsonParameters,
                         const brion::uint32_ts& cells ) final
    {
        globalData.stimulusString = jsonParameters;
        globalData.cells = cells;
    }

    void injectMultipleStimuli( const std::string& jsonParameters,
                                const brion::uint32_ts& cells ) final
    {
        globalData.stimulusString = jsonParameters;
        globalData.cells = cells;
    }

    void play()
    {
        globalData.playbackState = SimulatorData::PLAY;

    }

    void pause()
    {
        globalData.playbackState = SimulatorData::PAUSE;
    }
};

lunchbox::PluginRegisterer< DummySimulator > registerer;


BOOST_AUTO_TEST_CASE( test_create_handled_simulator )
{
    monsteer::URI uri( "dummy://" );
    monsteer::Simulator simulator( uri );

    BOOST_CHECK_EQUAL( globalData.subscriber, uri );
}

BOOST_AUTO_TEST_CASE( test_create_unhandled )
{
    BOOST_CHECK_THROW( monsteer::Simulator( monsteer::URI( "invalid://" )),
                       std::runtime_error );
}

monsteer::URI uri( "dummy://" );
uint32_t cellIds[] = {3, 7, 12, 20, 24, 30, 28, 1};
const std::string jsonParameters( "blahblah" );

namespace std
{
    ostream& operator<<( ostream& os, const brion::uint32_ts& cellIds )
    {
        for( const uint32_t& cellId : cellIds )
            os << cellId << " ";
        return os << std::endl;
    }
}

BOOST_AUTO_TEST_CASE( test_inject_stimulus )
{
    monsteer::Simulator simulator( uri );

    brion::uint32_ts cells;
    for( uint32_t i = 0; i != 8; ++i )
        cells.push_back( cellIds[i] );

    simulator.injectStimulus( jsonParameters, cells );
    BOOST_CHECK_EQUAL( globalData.stimulusString, jsonParameters );
    BOOST_CHECK_EQUAL( globalData.cells, cells );

    globalData.cells.clear();
}

BOOST_AUTO_TEST_CASE( test_inject_multiple_stimuli )
{
    monsteer::Simulator simulator( uri );

    brion::uint32_ts cells;
    for( uint32_t i = 0; i != 8; ++i )
        cells.push_back( cellIds[i] );

    simulator.injectMultipleStimuli( jsonParameters, cells );
    BOOST_CHECK_EQUAL( globalData.stimulusString, jsonParameters );
    BOOST_CHECK_EQUAL( globalData.cells, cells );

    globalData.cells.clear();
}


BOOST_AUTO_TEST_CASE( test_play_pause )
{
    monsteer::Simulator simulator( uri );

    simulator.play();
    BOOST_CHECK( globalData.playbackState == SimulatorData::PLAY );

    simulator.pause();
    BOOST_CHECK( globalData.playbackState == SimulatorData::PAUSE );
}
