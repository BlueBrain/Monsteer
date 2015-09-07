[TOC]

# Introduction

Monsteer is a library for Interactive Supercomputing in the neuroscience domain.
Monsteer facilitates the coupling of running simulations (currently NEST) with
interactive visualization and analysis applications. Monsteer support streaming
of simulation data to clients (currenty only spikes) as well as control of the
simulator from the clients (also kown as computational steering).
Monsteer's main components are a C++ library, an MUSIC-based application and Python
helpers

## Features

Monsteer provides the following functionality:
* A brion::SpikeReportPlugin for streaming spike data using ZeroEQ. The
  plugin accepts URIs with the format "monsteer://[host[:port]]".
* A MUSIC application called music_proxy to be used as the runtime gateway
  to simulators that support MUSIC, e.g. NEST.
* A small Python library to interface the Simulator in the client side and
  MUSIC proxy on the simulator side. This library also activates the Brion
  plugin when imported.

## Examples

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

## Building from Source

~~~
git clone https://github.com/BlueBrain/Monsteer.git
mkdir Monsteer/build
cd Monsteer/build
cmake ..
make
~~~

## Detailed documentation

- [User guide](@ref User_Guide)
- [Technical overview](@ref Technical_Overview)

Contact: ahmet.bilgili@epfl.ch
         jafet.villafrancadiaz@epfl.ch
         jhernando@fi.upm.es
