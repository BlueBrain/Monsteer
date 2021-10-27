Monsteer
=======

# Overview

Monsteer is a library for Interactive Supercomputing in the neuroscience
domain. Monsteer facilitates the coupling of running simulations
(currently NEST) with interactive visualization and analysis
applications. Monsteer supports streaming of simulation data to clients
(currenty only spikes) as well as control of the simulator from the
clients (also kown as computational steering). Monsteer's main
components are a C++ library, a MUSIC-based application and Python
helpers.

# Features

Monsteer provides the following functionality:
* A brion::SpikeReportPlugin for streaming spike data using ZeroEQ. The
  plugin accepts URIs with the format "monsteer://[host[:port]]".
* A MUSIC application called music_proxy to be used as the runtime gateway
  to simulators that support MUSIC, e.g. NEST.
* A small Python library to interface the Simulator in the client side and
  MUSIC proxy on the simulator side. This library also activates the Brion
  plugin when imported.

# Examples

The directory *examples/nest* contains two simple examples using NEST. For each
example there is Python script (the simulator code) and a MUSIC configuration
file. The music configuration files have instructions on how to run them. The
first example, (files *minimal.py* and *nest2music_proxy.music*), is a minimal
simulation with streaming enabled. The second example (*minimal_steered.py*
and *nest2music_proxy_with_steering.music*) is an extended version of the
first one that adds basic steering support.

This package does not provide any client code example at the moment. As a
streaming client you can use the spikeConverter tool from
[Brion](https://github.com/BlueBrain/Brion.git) as a reference.

# Building from Source

Monsteer is a cross-platform library, designed to run on any modern operating
system, including all Unix variants. It requires a C++11 compiler and uses CMake
to create a platform-specific build environment. The following platforms and
build environments are tested:

* Linux: Ubuntu 16.04, RHEL 6.8 (Makefile, Ninja)
* Mac OS X: 10.9 (Makefile, Ninja)

Building from source is as simple as:

    git clone --recursive https://github.com/BlueBrain/Monsteer.git
    mkdir Monsteer/build
    cd Monsteer/build
    cmake -GNinja -DCLONE_SUBPROJECTS=ON ..
    ninja

# Funding & Acknowledgment
 
The development of this software was supported by funding to the Blue Brain Project,
a research center of the École polytechnique fédérale de Lausanne (EPFL), from the
Swiss government’s ETH Board of the Swiss Federal Institutes of Technology.

This project has received funding from the European Union’s FP7-ICT programme
under Grant Agreement No. 604102 (Human Brain Project RUP).

This project has received funding from the European Union's Horizon 2020 Framework
Programme for Research and Innovation under the Specific Grant Agreement No. 720270
(Human Brain Project SGA1).

This project is based upon work supported by the King Abdullah University of Science
and Technology (KAUST) Office of Sponsored Research (OSR) under Award No. OSR-2017-CRG6-3438.

# License

Monsteer is licensed under the LGPL, unless noted otherwise, e.g., for external dependencies.
See file LICENSE.txt for the full license. 

Copyright (c) 2015-2021 Blue Brain Project/EPFL, King Abdullah University of Science and
Technology and contributors.

This library is free software; you can redistribute it and/or modify it under the terms of the
GNU Lesser General Public License version 3 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library;
if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301 USA

