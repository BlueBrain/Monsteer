
/* Copyright (c) 2006-2015, Ahmet Bilgili <ahmet.bilgili@epfl.ch>
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

#define MUSIC_TIMESTEP 0.001f //seconds

#include <lunchbox/lunchbox.h>

#include <mpi.h>
#include <music.hh>

#include <boost/scoped_ptr.hpp>
#include <algorithm>

namespace
{
const std::string eventPortName = "outSpikes";
const float cellFirePercentage = 0.05f;
}

class MusicSpikeWriter : public boost::noncopyable
{
public:
    MusicSpikeWriter( int32_t argc, char* argv[] )
        : _musicSetup( argc, argv )
    {
        _setUpMusic();
    }

    void run()
    {
        LBVERB << "Application loop" << std::endl;

        while( _runtime->time() < _stoptime )
        {
            // Shuffle cells
            std::random_shuffle( _cellShuffle.begin(), _cellShuffle.end( ));
            const size_t percentageCount =
                _cellShuffle.size() * cellFirePercentage;
            for( size_t i = 0; i < percentageCount; ++i )
                _eventPort->insertEvent( _runtime->time(),
                                      MUSIC::GlobalIndex( _cellShuffle[ i ] ));
             lunchbox::sleep( MUSIC_TIMESTEP * 1000 );
             _runtime->tick();
        }
        _runtime->finalize();
    }

private:
    MUSIC::Setup _musicSetup;
    boost::scoped_ptr< MUSIC::Runtime > _runtime;
    MUSIC::EventOutputPort* _eventPort;
    std::vector< uint32_t > _cellShuffle;
    boost::scoped_ptr< MUSIC::IndexMap > _indexMap;
    double _stoptime;

    void _setUpMusic()
    {
        MPI::Intracomm communicator = _musicSetup.communicator();
        const int32_t rank = communicator.Get_rank();
        _eventPort = _musicSetup.publishEventOutput( eventPortName );

        if ( !_eventPort->isConnected( ))
        {
            if( rank == 0 )
                LBERROR << "Music_sender: no connected output ports"
                        << std::endl;
            communicator.Abort(1);
        }

        if ( !_eventPort->hasWidth( ))
        {
            LBERROR << "Music_sender: Port width not specified in "
                        "Configuration file" << std::endl;
            communicator.Abort(1);
        }

        // Linear map with the cell GIDs of this process.
        // The width is split among the available processes.
        const int32_t processes = communicator.Get_size();
        const int32_t width = _eventPort->width();
        const int32_t perProc = width / processes;
        const int32_t remainder = width % processes;
        const int32_t firstId =
            // The remainder is assigned one-by-one to the first n processors
            perProc * rank + 1 + std::min( rank, remainder );
        const int32_t count =
            perProc + ( remainder > rank ? remainder == 0 : 0 );
        _cellShuffle.resize( count );
        std::iota( _cellShuffle.begin(), _cellShuffle.end(), firstId );
        _indexMap.reset( new MUSIC::LinearIndex( firstId, count ));

        // Mapping the port
        _eventPort->map( _indexMap.get(), MUSIC::Index::GLOBAL );

        _musicSetup.config( "stoptime", &_stoptime );

        LBINFO << "Starting MUSIC runtime" << std::endl;
        _runtime.reset( new MUSIC::Runtime( &_musicSetup, MUSIC_TIMESTEP ));
    }
};

int32_t main( int32_t argc, char *argv[] )
{
    MusicSpikeWriter writer( argc, argv );
    writer.run();
    return EXIT_SUCCESS;
}
