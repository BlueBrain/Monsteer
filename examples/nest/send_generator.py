#!/usr/bin/env python

## Copyright (c) 2014-2015 BBP/EPFL
## All rights reserved. Do not distribute without permission.
##
## Responsible Authors: Juan Hernando <jhernando@fi.upm.es>

# This script instantiates a NEST steering proxy and sends a stimulus
# injection event after a 4 second pause. If not run from the command line,
# the event is not sent, but the event parameters are still set.
# The function send_stimulus can be invoked to send the predefined stimulus
# injection.

import monsteer.Nesteer
import bbp
import nest
import numpy

simulator = isc.Nesteer.Simulator('monsteer-nesteer://')

params = nest.GetDefaults('poisson_generator')
params['rate'] = 50000.0

cells = bbp.Cell_Target()
for i in range(1000):
    cells.insert(i + 1)

def send_stimulus():
    simulator.injectStimulus(params, cells)

if __name__ == '__main__':
    import time
    time.sleep(4) # Waiting for connection
    send_stimulus()
