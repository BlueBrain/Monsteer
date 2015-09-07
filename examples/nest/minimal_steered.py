#!/usr/bin/env python

## Copyright (c) 2014-2015 BBP/EPFL
## All rights reserved. Do not distribute without permission.
##
## Responsible Authors: Ahmet Bilgili <ahmet.bilgili@epfl.ch>
##                      Juan Hernando <jhernando@fi.upm.es>

# This script is a minimal simulation setup for showing computational steering
# of NEST using music_proxy.
# A blank simulation is started and connected through MUSIC ports to
# music_proxy, which in turn uses zeq to send spikes out and receive the
# steering events to create a stimulus generator to apply to the cells.

duration = 100000
cell_count = 1000

from monsteer.Nesteer import Nesteer
from nest import *

neurons = Create("iaf_neuron", cell_count)

isc_communicator = Nesteer('spikes_out',
                           'steering_input',
                           neurons,
                           [i + 1 for i in range(cell_count)])

def apply_spikes(generator, neurons):
    Connect(generator, neurons, 'all_to_all', syn_spec={'weight': 40.0})
isc_communicator.set_apply_generators_to_neurons_function(apply_spikes)

time = 0
while time < duration:
    Simulate(10)
    isc_communicator.process_events()
    time += 10
