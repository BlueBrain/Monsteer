
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
#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>

#include <thread>

#define NEST_SPIKES_START_TIME 1.8f
#define NEST_SPIKES_END_TIME 98.8f
#define NEST_SPIKES_COUNT 96256
#define NEST_SPIKE_REPORT_FILE "NESTSpikeData/spike_detector-65537-00.gdf"

#define STARTUP_DELAY 250

#define INTER_SPIKE_INTERVAL std::chrono::milliseconds(100)

const lunchbox::URI uri(MONSTEER_BRION_SPIKES_PLUGIN_SCHEME + "://127.0.0.1");

// Explicit registration required because the folder of the brion plugin is not
// in the LD_LIBRARY_PATH of the test executable.
lunchbox::PluginRegisterer<monsteer::plugin::SpikeReport> registerer;

using State = brion::SpikeReport::State;

inline void debugSpikes(const brion::Spikes& v, const char* prefix = "")
{
    for (auto& spike : v)
    {
        std::cout << prefix << spike.first << " -- " << spike.second << "\n";
    }

    std::cout << std::flush;
}

const brion::Spikes& getTestSpikes()
{
    static const brion::Spikes spikes = {
        {0.1f, 20}, {0.2f, 22}, {0.25f, 23}, {0.3f, 24}, {0.4f, 25}};

    return spikes;
}

BOOST_AUTO_TEST_CASE(invoke_invalid_method_streaming)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    BOOST_CHECK_THROW(receiver.write(brion::Spikes{}), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(write_read)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    std::thread writeThread{[&emitter] {
        for (const brion::Spike& spike : getTestSpikes())
        {
            std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
            emitter.write({spike});
        }
        emitter.close();
    }};

    brion::Spikes readSpikes;

    for (size_t i = 1; i < getTestSpikes().size(); ++i)
    {
        if (receiver.getState() != brion::SpikeReport::State::ended)
        {
            auto tmpSpikes = receiver.read(getTestSpikes()[i].first).get();
            readSpikes.insert(readSpikes.end(), tmpSpikes.begin(),
                              tmpSpikes.end());
        }
    }

    if (receiver.getState() != brion::SpikeReport::State::ended)
    {
        auto tmpSpikes = receiver.read(brion::UNDEFINED_TIMESTAMP).get();
        readSpikes.insert(readSpikes.end(), tmpSpikes.begin(), tmpSpikes.end());
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(getTestSpikes().begin(),
                                  getTestSpikes().end(), readSpikes.begin(),
                                  readSpikes.end());

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(write_read_until)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    std::thread writeThread{[&emitter] {
        for (const brion::Spike& spike : getTestSpikes())
        {
            std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
            emitter.write({spike});
        }
        emitter.close();
    }};

    brion::Spikes readSpikes;

    for (size_t i = 1; i < getTestSpikes().size(); ++i)
    {
        auto tmpSpikes = receiver.readUntil(getTestSpikes()[i].first).get();
        readSpikes.insert(readSpikes.end(), tmpSpikes.begin(), tmpSpikes.end());
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(getTestSpikes().begin(),
                                  getTestSpikes().end(), readSpikes.begin(),
                                  readSpikes.end());

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(seek)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    std::thread writeThread{[&emitter] {

        std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
        emitter.seek(10).get();
        std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
        emitter.seek(20).get();
        emitter.close();
    }};

    receiver.seek(10).get();
    BOOST_CHECK_EQUAL(receiver.getCurrentTime(), 10.f);
    BOOST_CHECK(receiver.getState() != brion::SpikeReport::State::ended);

    receiver.seek(20).get();
    BOOST_CHECK(receiver.getCurrentTime() == 20.f ||
                receiver.getState() == brion::SpikeReport::State::ended);

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(write_read_filtered)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI(), {20, 22}};

    lunchbox::sleep(STARTUP_DELAY);
    std::thread writeThread{[&emitter] {
        for (const brion::Spike& spike : getTestSpikes())
        {
            std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
            emitter.write({spike});
        }
        emitter.close();
    }};

    brion::Spikes readSpikes;

    for (size_t i = 1; i < getTestSpikes().size(); ++i)
    {
        if (receiver.getState() != brion::SpikeReport::State::ended)
        {
            auto tmpSpikes = receiver.read(getTestSpikes()[i].first).get();
            readSpikes.insert(readSpikes.end(), tmpSpikes.begin(),
                              tmpSpikes.end());
        }
    }

    if (receiver.getState() != brion::SpikeReport::State::ended)
    {
        auto tmpSpikes = receiver.read(brion::UNDEFINED_TIMESTAMP).get();
        readSpikes.insert(readSpikes.end(), tmpSpikes.begin(), tmpSpikes.end());
    }

    BOOST_CHECK_EQUAL(readSpikes.size(), 2);

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(write_read_until_filtered)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI(), {20, 22}};
    lunchbox::sleep(STARTUP_DELAY);

    std::thread writeThread{[&emitter] {
        for (const brion::Spike& spike : getTestSpikes())
        {
            std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
            emitter.write({spike});
        }
        emitter.close();
    }};

    brion::Spikes readSpikes;

    for (size_t i = 1; i < getTestSpikes().size(); ++i)
    {
        auto tmpSpikes = receiver.readUntil(getTestSpikes()[i].first).get();
        readSpikes.insert(readSpikes.end(), tmpSpikes.begin(), tmpSpikes.end());
    }

    BOOST_CHECK_EQUAL(readSpikes.size(), 2);

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(simultaneous_read)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    auto spikes = receiver.read(brion::UNDEFINED_TIMESTAMP);

    BOOST_CHECK_THROW(receiver.read(brion::UNDEFINED_TIMESTAMP),
                      std::runtime_error);

    emitter.close();
}

BOOST_AUTO_TEST_CASE(invalid_read)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};
    lunchbox::sleep(STARTUP_DELAY);

    std::thread writeThread{[&emitter] {
        for (const brion::Spike& spike : getTestSpikes())
        {
            std::this_thread::sleep_for(INTER_SPIKE_INTERVAL);
            emitter.write({spike});
        }
        emitter.close();
    }};

    auto readSpikes = receiver.readUntil(0.3).get();

    // This doesn't throw anymore
    receiver.read(0.1);

    BOOST_CHECK_THROW(receiver.readUntil(0.1), std::logic_error);

    writeThread.join();
}

BOOST_AUTO_TEST_CASE(invalid_write)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    emitter.write(getTestSpikes());
    BOOST_CHECK_THROW(emitter.write(getTestSpikes()), std::logic_error);
}

BOOST_AUTO_TEST_CASE(forward)
{
    // file ==> emitter ==> receiver ==> writer (file)

    auto sourceSpikesFile =
        (boost::filesystem::path(BBP_TESTDATA) / NEST_SPIKE_REPORT_FILE)
            .string();

    auto destinationSpikesFile =
        "/tmp/" + lunchbox::make_UUID().getString() + ".gdf";

    {
        brion::SpikeReport sourceReport{brion::URI{sourceSpikesFile}};
        brion::SpikeReport emitter{uri, brion::MODE_WRITE};
        brion::SpikeReport receiver{emitter.getURI()};
        brion::SpikeReport writer{brion::URI{destinationSpikesFile},
                                  brion::MODE_WRITE};

        lunchbox::sleep(STARTUP_DELAY);

        std::thread fileToStreamThread{[&sourceReport, &emitter] {
            float t = NEST_SPIKES_START_TIME;
            while (sourceReport.getState() == State::ok)
            {
                auto spikes = sourceReport.read(t).get();
                t = std::max(sourceReport.getCurrentTime(), t + 1);
                emitter.write(spikes);
            }
            emitter.close();
        }};

        std::thread streamToFileThread{[&receiver, &writer] {
            float t = NEST_SPIKES_START_TIME;
            while (receiver.getState() == State::ok)
            {
                auto spikes = receiver.read(t).get();
                t = std::max(receiver.getCurrentTime(), t + 1);
                writer.write(spikes);
            }
        }};

        fileToStreamThread.join();
        streamToFileThread.join();
    }

    // checks
    {
        brion::SpikeReport sourceReport{brion::URI{sourceSpikesFile}};
        brion::SpikeReport destinationReport{brion::URI{destinationSpikesFile}};

        auto sourceSpikes = sourceReport.read(brion::UNDEFINED_TIMESTAMP).get();
        auto destinationSpikes =
            destinationReport.read(brion::UNDEFINED_TIMESTAMP).get();

        BOOST_REQUIRE_EQUAL(sourceSpikes.size(), destinationSpikes.size());
        BOOST_REQUIRE_EQUAL(NEST_SPIKES_COUNT, destinationSpikes.size());

        BOOST_CHECK_EQUAL_COLLECTIONS(sourceSpikes.begin(), sourceSpikes.end(),
                                      destinationSpikes.begin(),
                                      destinationSpikes.end());
    }
}

BOOST_AUTO_TEST_CASE(interrupt)
{
    brion::SpikeReport emitter{uri, brion::MODE_WRITE};
    brion::SpikeReport receiver{emitter.getURI()};

    auto future = receiver.read(100);
    lunchbox::sleep(100);
    receiver.interrupt();
    BOOST_CHECK_THROW(future.get(), std::runtime_error);

    future = receiver.read(100);
    receiver.interrupt();
    BOOST_CHECK_THROW(future.get(), std::runtime_error);
}
