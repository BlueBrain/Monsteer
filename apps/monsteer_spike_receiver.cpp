/* Copyright (c) 2006-2017, Ahmet Bilgili <ahmet.bilgili@epfl.ch>
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
#include <brion/brion.h>
#include <lunchbox/file.h>


namespace
{
const std::string pluginScheme(MONSTEER_BRION_SPIKES_PLUGIN_SCHEME + "://");
}

class CommandLineOptions
{
public:
    brion::URI inputURI;

    CommandLineOptions(int32_t argc, char* argv[])
        : inputURI(pluginScheme)
    {
        if( argc > 1 )
        {
            if( std::string( argv[1] ) == "--help" )
            {
                std::cout << lunchbox::getFilename( argv[0] )
                          << " [hostname]: receive spike stream and output to "
                          << "stdout" << std::endl;
                ::exit( EXIT_SUCCESS );
            }
        }
        else
            inputURI = brion::URI( pluginScheme + argv[1] );
    }
};

class SpikeReceiver
{
public:
    SpikeReceiver(int32_t argc, char* argv[])
        : _options(argc, argv)
        , _reader(_options.inputURI, brion::MODE_READ)
    {
    }

    void run()
    {
        const float step = 10.f; // ms, arbitrary value
        while (_reader.getState() == brion::SpikeReport::State::ok)
        {
            const auto spikes =
                _reader.readUntil(_reader.getCurrentTime() + step).get();

            for (const brion::Spike& spike : spikes)
                std::cout << spike.first << " " << spike.second << std::endl;
        }
    }

private:
    CommandLineOptions _options;
    brion::SpikeReport _reader;
};

int32_t main(int32_t argc, char* argv[])
{
    SpikeReceiver receiver(argc, argv);
    receiver.run();
    return EXIT_SUCCESS;
}
