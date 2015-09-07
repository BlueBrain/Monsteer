#!/usr/bin/env python

## Copyright (c) 2014-2015 BBP/EPFL
## All rights reserved. Do not distribute without permission.
##
## Responsible Author: Ahmet Bilgili <ahmet.bilgili@epfl.ch>

# Example code to generate spikes using nest simulator

duration = 10000
cell_count = 1000

from nest import *

sli_run("statusdict/have_music ::")
if not spp():
    import sys
    print("NEST was not compiled with support for MUSIC, not running.")
    sys.exit()

spike_generators = Create("poisson_generator",
                          cell_count, params={"rate": 50000.0})

neurons = Create("iaf_neuron", cell_count)

Connect(spike_generators, neurons, 'one_to_one',
        {"model": "static_synapse", "weight":2.5, "delay":0.5})

meop = Create('music_event_out_proxy', 1)
SetStatus(meop, {'port_name': 'spikes_out'})
for i in range(cell_count):
    Connect([neurons[i]], meop, 'one_to_one', {"music_channel": i + 1})

time = 0
while time < duration:
    Simulate(10)
    time += 10

