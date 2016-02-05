
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

#include <monsteer/types.h>
#include <boost/noncopyable.hpp>

namespace monsteer
{

/** Steering interface to the simulator of an experiment.
 *
 * This class provides access to the simulator engine for computational
 * steering.
 *
 * The implementation is chosen automatically based on the URI that locates
 * the simulator in the network.
 *
 * @version 0.2
 */
class Simulator : public boost::noncopyable
{
public:
    /**
     * Create a simulator using the input URI as the location of the simulation
     * reply publisher.
     * The URI for the steering request publisher is derived from the input URI.
     *
     * @param uri A subscriber URI to listen to reply events.
     * @param uri B publisher URI to create steering events.
     * @throw std::runtime_error if the input URI is not handled by any
     *        registered simulator steering plugin
     * @version 0.2
     */

    explicit Simulator( const URI& uri);

    ~Simulator();

    /**
     * Attach a stimulus generator to a list of cells.
     *
     * A stimulus generator is created on the simulation side using the
     * parameters provided. The stimulus source is connected to all cells
     * at the moment it is received by the simulator.
     *
     * This operation may fail silently in the current implementation.
     *
     * @throw std::runtime_error if an error is detected in generator
     * parameters
     * @version 0.2
     */
    void injectStimulus( const std::string& jsonParameters,
                         const brion::uint32_ts& cells );


    /**
     * Attach multiple stimulus generators to a list of cells.
     *
     * Multiple stimulus generators (as many as cells) are created on the
     * simulation side using the parameters provided.
     * Each of the stimulus sources is connected to a different cell at the
     * moment they are received by the simulator.
     *
     * This operation may fail silently in the current implementation.
     *
     * @throw std::runtime_error if an error is detected in generator
     * parameters
     * @version 0.2
     */
    void injectMultipleStimuli( const std::string& jsonParameters,
                                const brion::uint32_ts& cells );

    /*
     * Resumes/plays the simulation.
     *
     * The synchronization policies are defined by each simulation engine and
     * plugin. The only guarantee provided is that if the timestamp that is
     * externally visible at any given moment is x and, the simulation will
     * be stopped at time y >= x with no additional guarantee about
     * the difference between y and x.
     *
     * Does not check the current playback state of the simulation.
     *
     * @version 0.2
     *
     */
    void play();

    /*
     * Pauses the simulation.
     *
     * Does not check the current playback state of the simulation.
     *
     * @version 0.2
     *
     */
    void pause();

    /*
     * Runs the simulation for a fixed duration.
     *
     * The specified duration applies only to the internal music-timer in the 
     * HBP music proxy application. The same restrictions as with play() apply.
     *
     * @version 0.3
     *
     *
     */
    void simulate( const double duration );


private:
    class Impl;
    Impl* _impl;
};

}
