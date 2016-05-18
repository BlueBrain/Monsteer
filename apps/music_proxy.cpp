
/* Copyright (c) 2006-2015, Ahmet Bilgili <ahmet.bilgili@epfl.ch>
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

#include "monsteer/playbackState_generated.h"
#include "monsteer/steering/vocabulary.h"
#include "monsteer/stimulus_generated.h"

#include <brion/spikeReport.h>
#include <lunchbox/debug.h>
#include <lunchbox/log.h>

#include <zeroeq/subscriber.h>
#include <zeroeq/publisher.h>
#include <zeroeq/vocabulary.h>

#include <flatbuffers/idl.h>

#include <music.hh>

#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include <functional>

namespace po = boost::program_options;

namespace
{
const double defaultMusicTimestep = 0.0001;
const brion::URI pluginURI( MONSTEER_BRION_SPIKES_PLUGIN_SCHEME + "://" );
}

class SpikesHandler : public MUSIC::EventHandlerGlobalIndex
{
public:
    SpikesHandler( MUSIC::Setup* setup, const std::string& spikesPort )
        : _spikeReport( pluginURI, brion::MODE_OVERWRITE )
    {
        LBINFO << "Initializing Spikes Handler" << std::endl;

        MPI::Intracomm communicator = setup->communicator();
        const uint32_t rank = communicator.Get_rank();
        // Publishing the ports
        _inSpikes = setup->publishEventInput( spikesPort );

        if( !_inSpikes->isConnected( ))
        {
            if( rank == 0 )
                LBERROR << "Spikes input port is not connected" << std::endl;
           communicator.Abort( 1 );
        }

        if( !_inSpikes->hasWidth( ))
        {
            if( rank == 0 )
                LBERROR << "Spikes input port does not have width"
                        << std::endl;
           communicator.Abort( 1 );
        }

        // Mapping the inSpikes port
        const uint32_t width = _inSpikes->width();
        _spikesMap.reset(new MUSIC::LinearIndex( 1, width ));
        _inSpikes->map( _spikesMap.get(), this, 0 );

        LBINFO << "Initialized Spikes Handler" << std::endl;
    }

    void operator()( double time, MUSIC::GlobalIndex gid ) final
    {
        _spikeBuffer.insert( std::make_pair( float( time * 1000 ),
                                             uint32_t( gid )));
    }

    void flush()
    {
        if (_spikeBuffer.empty())
            return;

        _spikeReport.writeSpikes( _spikeBuffer );
        _spikeBuffer.clear();
    }

private:

    MUSIC::EventInputPort* _inSpikes;
    brion::SpikeReport _spikeReport;
    brion::Spikes _spikeBuffer;
    boost::scoped_ptr< MUSIC::IndexMap > _spikesMap;
};

class CommandLineOptions
{
public:
    std::string spikesPort;
    std::string steeringPort;

    bool enableSteering;

    double musicTimestep;

    CommandLineOptions( int32_t argc, char* argv[] )
        : enableSteering( false )
        , musicTimestep( defaultMusicTimestep )
    {
        bool showHelp;

        po::options_description options( "MUSIC proxy" );

        options.add_options()
            ( "help,h", po::bool_switch(&showHelp)->default_value( false ),
              "produce help message" )
            ( "timestep",
              po::value< double >(
                  &musicTimestep )->default_value( defaultMusicTimestep ),
              "MUSIC library tick time steps in sec." )

            ( "music-streaming-port",
              po::value< std::string >(
                  &spikesPort )->default_value( "spikesPort" ),
              "MUSIC streaming port" )

            ( "music-steering-port",
              po::value< std::string >(
                  &steeringPort )->default_value( "steeringPort" ),
              "MUSIC steering port" )

            ( "steering",
              po::bool_switch(&enableSteering)->default_value( false ),
              "Enable steering. This creates the ZeroEQ subscriber and "
              "announces the MUSIC message ports" );

        options.add_options();

        po::variables_map variableMap;

        try
        {
            // parse program options, ignore all non related options
            po::store( po::command_line_parser( argc, argv ).options(
                           options ).allow_unregistered().run(),
                       variableMap );
            po::notify( variableMap );
        }
        catch( std::exception& exception )
        {
            LBERROR << "Error parsing command line: " << exception.what()
                    << std::endl;
            ::exit( EXIT_FAILURE );
        }

        if( showHelp )
        {
            std::cout << options << std::endl;
            ::exit( EXIT_SUCCESS );
        }
    }
};

class SteeringHandler
{
public:

    SteeringHandler( MUSIC::Setup* setup, const std::string& steeringPort )
        : _state( monsteer::steering::SimulationPlaybackState::PLAY )
        , _stimuliInjectionEvent( monsteer::steering::EVENT_STIMULUSINJECTION,
         [&]( const zeroeq::FBEvent& event ){ _onStimulusInjection( event); })
        , _playbackStateEvent( monsteer::steering::EVENT_PLAYBACKSTATE,
         [&]( const zeroeq::FBEvent& event ){ _onPlaybackStateChange( event); })
    {
        LBINFO << "Initializing Steering Handler" << std::endl;

        _steeringOutput = setup->publishMessageOutput( steeringPort );
        if( !_steeringOutput->isConnected( ))
        {
            if( setup->communicator().Get_rank() == 0 )
                LBERROR << "Steering port is not connected" << std::endl;
             setup->communicator().Abort( 1 );
        }

        _steeringOutput->map();
        _subscriber.subscribe( _stimuliInjectionEvent );
        _subscriber.subscribe( _playbackStateEvent );

        LBINFO << "Initialized Steering Handler" << std::endl;
    }

    void processMessages( const double musicTime )
    {
        switch( _state )
        {
        case monsteer::steering::SimulationPlaybackState::PLAY:
        {
            _currentTime = musicTime;
            while( _subscriber.receive( 0 ))
             ;
            break;
        }
        case monsteer::steering::SimulationPlaybackState::PAUSE:
        {
            _subscriber.receive( -1 );
            break;
        }
        }
    }

    monsteer::steering::SimulationPlaybackState::State getPlaybackState() const
    {
        return _state;
    }

    std::string deserializeJSON( const zeroeq::FBEvent& event,
                                 const std::string& schema )
    {
         flatbuffers::Parser parser;
         if( !parser.Parse( schema.c_str( )))
            throw std::runtime_error( parser.error_ );

         std::string json;
         flatbuffers::GeneratorOptions opts;
         opts.base64_byte_array = true;
         opts.strict_json = true;
         GenerateText( parser, event.getData(), opts, &json );
         return json;
    }

private:

    void _onStimulusInjection( const zeroeq::FBEvent& event )
    {
        LBASSERT( event.getTypeIdentifier() == monsteer::steering::EVENT_STIMULUSINJECTION )
        const std::string& json = deserializeJSON( event,
                                monsteer::steering::SCHEMA_STIMULUSINJECTION );
        // Although music library internally "does not" touch the sent data,
        // the Music "insertMessage" function accepts only non-const data.
        // Not to have a second copy, the below const_cast is applied.
        _steeringOutput->insertMessage( _currentTime,
                                        const_cast<char*>(json.c_str()),
                                        json.size());
    }

    void _onPlaybackStateChange( const zeroeq::FBEvent& event )
    {
        LBASSERT( event.getTypeIdentifier() == monsteer::steering::EVENT_PLAYBACKSTATE )
        const monsteer::steering::SimulationPlaybackState& state =
                monsteer::steering::deserializePlaybackState( event );

        _state = state.state;
    }

    zeroeq::Subscriber _subscriber;
    MUSIC::MessageOutputPort* _steeringOutput;
    double _currentTime;
    monsteer::steering::SimulationPlaybackState::State _state;
    zeroeq::FBEvent _stimuliInjectionEvent;
    zeroeq::FBEvent _playbackStateEvent;
};

class Proxy : boost::noncopyable
{
public:

    // Application command line parsing ( CommandLineOptions() initialization )
    // must go after MUSIC::Setup because it modifies argc and argv to bring
    // in the application arguments from the config file.
    Proxy( int32_t& argc, char**& argv )
        : _setup( new MUSIC::Setup( argc, argv ))
        , _communicator( _setup->communicator( ))
        , _rank( _communicator.Get_rank( ))
        , _options( argc, argv )
        , _spikesHandler( _setup, _options.spikesPort )
    {
        if( _options.enableSteering )
        {
            _steeringHandler.reset(
                        new SteeringHandler( _setup, _options.steeringPort ));
        }

        _setup->config( "stoptime", &_stoptime );
    }

    void run()
    {
        LBINFO << "Starting MUSIC runtime" << std::endl;
        MUSIC::Runtime runtime( _setup, _options.musicTimestep );
        // The constructor has called delete _setup.
        _setup = 0;

        LBVERB << "Application loop" << std::endl;

        double apptime = runtime.time();
        while( apptime < _stoptime )
        {
            if( !_steeringHandler ||
                 _steeringHandler->getPlaybackState() ==
                    monsteer::steering::SimulationPlaybackState::PLAY )
                runtime.tick();

            if( _steeringHandler )
                _steeringHandler->processMessages(apptime);

            _spikesHandler.flush();
            apptime = runtime.time();
        }

        runtime.finalize();
    }

private:
    // MUSIC and MPI handlers
    MUSIC::Setup* _setup; // Implictly deleted by the _runtime object
    MPI::Intracomm _communicator;
    uint32_t _rank;

    CommandLineOptions _options;

    double _stoptime;

    // Internal data structures
    SpikesHandler _spikesHandler;
    boost::scoped_ptr< SteeringHandler > _steeringHandler;
};

int32_t main( int32_t argc, char* argv[] )
{
    Proxy proxy( argc, argv );
    proxy.run();
    return EXIT_SUCCESS;
}
