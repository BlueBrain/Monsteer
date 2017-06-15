
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

#include <boost/python.hpp>

#include "monsteer/steering/simulator.h"

#include <boost/python/stl_iterator.hpp>

using namespace monsteer;
using namespace boost::python;

void Simulator_injectStimulus(monsteer::Simulator& simulator,
                              const std::string& json, object list)
{
    brion::uint32_ts ids;
    ids.reserve(len(list));
    stl_input_iterator<uint32_t> i(list), end;
    while (i != end)
        ids.push_back(*i++);
    simulator.injectStimulus(json, ids);
}

void Simulator_injectMultipleStimuli(monsteer::Simulator& simulator,
                                     const std::string& json, object list)
{
    brion::uint32_ts ids;
    ids.reserve(len(list));
    stl_input_iterator<uint32_t> i(list), end;
    while (i != end)
        ids.push_back(*i++);
    simulator.injectMultipleStimuli(json, ids);
}

SimulatorPtr Simulator_init(const std::string& uri)
{
    return SimulatorPtr(new Simulator(servus::URI(uri)));
}

void export_Simulator()
{
    class_<Simulator, boost::noncopyable>("Simulator", no_init)
        .def("__init__", make_constructor(Simulator_init))
        .def("injectStimulus", Simulator_injectStimulus, arg("json"))
        .def("injectMultipleStimuli", Simulator_injectMultipleStimuli,
             arg("json"))
        .def("play", &Simulator::play)
        .def("pause", &Simulator::pause);
}
