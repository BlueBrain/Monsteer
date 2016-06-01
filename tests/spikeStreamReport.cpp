
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

#include <brain/brain.h>
#include <brain/spikeReportReader.h>
#include <brain/spikeReportWriter.h>
#include <monsteer/plugin/spikeReport.h>

#include <BBP/TestDatasets.h>

#include <brion/brion.h>
#include <brion/spikeReport.h>
#include <lunchbox/lunchbox.h>

#define BOOST_TEST_MODULE MONSTEER
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

#include <thread>

#define NEST_SPIKES_START_TIME 1.8f
#define NEST_SPIKES_END_TIME 98.8f
#define NEST_SPIKES_COUNT 96256
#define NEST_SPIKE_REPORT_FILE "NESTSpikeData/spike_detector-65537-00.gdf"

#define STARTUP_DELAY 250

const lunchbox::URI uri( MONSTEER_BRION_SPIKES_PLUGIN_SCHEME + "://127.0.0.1" );

// Explicit registration required because the folder of the brion plugin is not
// in the LD_LIBRARY_PATH of the test executable.
lunchbox::PluginRegisterer< monsteer::plugin::SpikeReport > registerer;

void spike_writer( brion::SpikeReport* writer )
{
    boost::filesystem::path path( BBP_TESTDATA );
    const std::string& files = NEST_SPIKE_REPORT_FILE;
    const brion::SpikeReport report( brion::URI(( path / files ).string( )),
                                     brion::MODE_READ );

    const brion::Spikes& spikes = report.getSpikes();
    std::set< float > timeSet;
    for( brion::Spikes::const_iterator it = spikes.begin();
         it != spikes.end(); ++ it )
        timeSet.insert( it->first );

    BOOST_FOREACH( const float time, timeSet )
    {
        brion::Spikes::const_iterator begin = spikes.lower_bound( time );
        brion::Spikes::const_iterator end = spikes.upper_bound( time );

        const brion::Spikes messageSpike( begin, end );
        writer->writeSpikes( messageSpike );
    }
    writer->close();
}

void read_chunks( brion::SpikeReport* reader )
{
    for( float time = 10; time < 101; time += 10 )
    {
        const bool status = reader->waitUntil( time );
        if( status )
            BOOST_CHECK( reader->getEndTime() <= time );
        else
        {
            BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
            BOOST_CHECK_EQUAL( reader->getEndTime(), NEST_SPIKES_END_TIME );
            BOOST_CHECK_EQUAL( reader->getSpikes().size(), NEST_SPIKES_COUNT );
        }
    }
}

void stream_clear_reader( brion::SpikeReport* reader )
{
    size_t lines[] = { 62, 1122, 1934, 2044 };
    double timestamps[] = { 1.8, 1.9, 2.0, 2.1 };

    reader->waitUntil( timestamps[3] );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(), lines[3] );

    reader->clear( timestamps[1], timestamps[2] );
    BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
    BOOST_CHECK_CLOSE( reader->getEndTime(), 2.1, 0.00001 );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(),
                       lines[3] - ( lines[1] - lines[0] )
                                - ( lines[2] - lines[1] ));

    reader->clear( timestamps[2], timestamps[3] );
    BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
    BOOST_CHECK_CLOSE( reader->getEndTime(), 1.8, 0.00001 );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(), lines[0] );

    reader->clear( 0, timestamps[0] );
    BOOST_CHECK_EQUAL( reader->getStartTime(), brion::UNDEFINED_TIMESTAMP );
    BOOST_CHECK_EQUAL( reader->getEndTime(), brion::UNDEFINED_TIMESTAMP );
    BOOST_CHECK( reader->getSpikes().empty( ));
}

void stream_read_all( brion::SpikeReport* reader )
{
    reader->waitUntil( brion::UNDEFINED_TIMESTAMP );

    BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
    BOOST_CHECK_EQUAL( reader->getEndTime(), NEST_SPIKES_END_TIME );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(), NEST_SPIKES_COUNT );
}

void stream_read_until_closed( brion::SpikeReport* reader )
{
    reader->waitUntil( brion::UNDEFINED_TIMESTAMP );

    BOOST_CHECK_EQUAL( reader->getStartTime(), brion::UNDEFINED_TIMESTAMP );
    BOOST_CHECK_EQUAL( reader->getEndTime(), brion::UNDEFINED_TIMESTAMP );
    BOOST_CHECK( reader->getSpikes().empty( ));
}

void stream_read_timeout( brion::SpikeReport* reader )
{
    BOOST_CHECK( !reader->waitUntil( 1000, 1 ));
}

void stream_read_by_chunks_with_timeout( brion::SpikeReport* reader )
{
    for( float time = 10; time < 101; ++time )
    {
        while( !reader->waitUntil( time, 1 ))
        {
            if( reader->getEndTime() == NEST_SPIKES_END_TIME )
                break;
        }
        BOOST_CHECK( reader->getEndTime() <= time );
    }
}

void stream_get_next_spike_time( brion::SpikeReport* reader )
{
    while( reader->waitUntil( reader->getNextSpikeTime( )))
        ;

    BOOST_CHECK( reader->getEndTime() == NEST_SPIKES_END_TIME );
}

template <typename T>
brion::URI getReadUri( const T& writer )
{
    brion::URI readUri = writer.getURI();
    readUri.setScheme( MONSTEER_BRION_SPIKES_PLUGIN_SCHEME );
    return readUri;
}

BOOST_AUTO_TEST_CASE( test_read_chunks )
{
    std::cout << "Running test_read_chunks" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( 250 );
    std::thread readerThread( std::bind( &read_chunks, &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_clear )
{
    std::cout << "Running test_stream_clear" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread( std::bind( &stream_clear_reader, &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_read_all )
{
    std::cout << "Running test_stream_read_all" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread( std::bind( &stream_read_all, &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_read_close )
{
    brion::SpikeReport reader( uri, brion::MODE_READ );
    std::thread readerThread( std::bind( &stream_read_until_closed,
                                             &reader ));
    reader.close();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_read_timeout )
{
    std::cout << "Running test_stream_read_timeout" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread( std::bind( &stream_read_timeout, &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_read_by_chunks_with_timeout )
{
    std::cout << "Running test_stream_read_by_chunks_with_timeout" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread(
        std::bind( &stream_read_by_chunks_with_timeout, &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( test_stream_get_next_spike_time )
{
    std::cout << "Running test_stream_get_next_spike_time" << std::endl;
    brion::SpikeReport writer( uri, brion::MODE_WRITE );
    brion::SpikeReport reader( getReadUri( writer ), brion::MODE_READ );
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread( std::bind( &stream_get_next_spike_time,
                                             &reader ));
    std::thread writerThread( std::bind( &spike_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

void spike_report_writer( brain::SpikeReportWriter* writer )
{
    boost::filesystem::path path( BBP_TESTDATA );
    const std::string& files = NEST_SPIKE_REPORT_FILE;
    brain::SpikeReportReader report( brion::URI(( path / files ).string( )));
    const brain::Spikes& spikes = report.getSpikes();
    writer->writeSpikes( spikes );
    writer->close();
}

void spike_report_reader( brain::SpikeReportReader* reader )
{
    while( !reader->hasEnded( ))
        reader->getSpikes();

    BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
    BOOST_CHECK_EQUAL( reader->getEndTime(), NEST_SPIKES_END_TIME  );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(), NEST_SPIKES_COUNT );
    BOOST_CHECK( reader->hasEnded( ));
}

void spike_timewindowed_reader( brain::SpikeReportReader* reader )
{
    reader->getSpikes(0, nextafterf( NEST_SPIKES_END_TIME, INFINITY ));
    BOOST_CHECK_EQUAL( reader->getStartTime(), NEST_SPIKES_START_TIME );
    BOOST_CHECK_EQUAL( reader->getEndTime(), NEST_SPIKES_END_TIME  );
    BOOST_CHECK_EQUAL( reader->getSpikes().size(), NEST_SPIKES_COUNT );
    BOOST_CHECK( reader->hasEnded( ));
}

BOOST_AUTO_TEST_CASE( test_spike_report_read_write )
{
    brain::SpikeReportWriter writer( uri );
    brain::SpikeReportReader reader( getReadUri( writer ));
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread( std::bind( &spike_report_reader, &reader ));
    std::thread writerThread( std::bind( &spike_report_writer, &writer ));
    writerThread.join();
    readerThread.join();
}

BOOST_AUTO_TEST_CASE( monsteer_spikes_time_windowed_read_write )
{
    brain::SpikeReportWriter writer( uri );
    brain::SpikeReportReader reader( getReadUri( writer ));
    lunchbox::sleep( STARTUP_DELAY );
    std::thread readerThread(
        std::bind( &spike_timewindowed_reader, &reader ));
    std::thread writerThread(
        std::bind( &spike_report_writer, &writer ));
    writerThread.join();
    readerThread.join();
}
