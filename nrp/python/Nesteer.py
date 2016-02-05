##
## Monsteer
## This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
##
## contact: ahmet.bilgili@epfl.ch
##          jafet.villafrancadiaz@epfl.ch


import nrp as _nrp

from six.moves import cPickle as _pickle
import itertools as _itertools
import json as _json
import nest as _nest
import numpy as _numpy
import os.path
import sys as _sys


def _apply_generators_to_neurons(generators, neurons):
    """
    Connect the spike generators to the neurons
    """
    print('Stimulus injected to cells: ' + str(neurons[0:10]) + (
                                         ' ...' if len(neurons) > 10 else ''))
    print('Stimulus: ' + str(_nest.GetStatus([generators[0]])))

    # If only one generator is present, connect it to all the neurons,
    # if more than one, it means that there is a different one for each cell
    if len(generators) == 1:
        _nest.Connect([generators[0]], neurons, 'all_to_all')
    elif len(generators) > 1:
        assert(len(generators) == len(neurons))
        _nest.Connect(generators, neurons, 'one_to_one')


class JSONEncoder(_json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, _nest.pynestkernel.SLILiteral):
            return [obj.name]
        if isinstance(obj, _numpy.ndarray) and obj.ndim == 1:
            return obj.tolist()
        return _json.JSONEncoder.default(self, obj)


class Simulator(_nrp.Simulator):
    """
    Class to create a NEST specific simulator proxy using the parent class from
    Monsteer. It hides details that are irrelevant for users, such as the
    json_enconder parameter.
    """
    def __init__(self, uri):
        self.simulator = _nrp.Simulator(uri)

    def injectStimulus(self, params, cells):
        """
        injectStimulus((Simulator)arg1, (dict)arg2, (Cell_Target)arg3) -> None
        """
        self.simulator.injectStimulus(params, cells, json_encoder=JSONEncoder)

    def injectMultipleStimuli(self, params, cells):
        """
        injectMultipleStimuli((Simulator)arg1, (dict)arg2, (Cell_Target)arg3) -> None
        """
        self.simulator.injectMultipleStimuli(params, cells,
                                             json_encoder=JSONEncoder)


    def play(self):
        """
        Plays/Resumes the simulation
        """
        self.simulator.play()


    def pause(self):
        """
        Pauses the simulation
        """
        self.simulator.pause()


    def simulate(self, duration):
        """
        Runs the simulator for the given amount of time in milliseconds.
        """
        self.simulator.simulate(duration)

        
        


class Nesteer:
    """
    The ISC communication class that handles MUSIC event ports for streaming
    spikes and message ports for steering input.
    """
    def __init__(self, spike_port_name, steering_port_name, neurons, gids, max_music_channel):
       self._spike_port_name = spike_port_name
       self._steering_port_name = steering_port_name
       self._apply_generators_to_neurons_function = _apply_generators_to_neurons
       self._generator_dict = {}
       self._gid_to_neuron_map = {}
       self._event_function_dict = {}
       self._max_music_channel = max_music_channel

       for (neuron, gid) in zip(neurons, gids):
            #status = _nest.GetStatus([neuron])
            #global_id = status[0]['global_id']
            global_id = neuron
            self._gid_to_neuron_map[gid] = global_id

       self._setup_music()

    def process_events(self):
        """
        Processes incoming events from steering port
        """
        event = _nest.GetStatus(self._music_steering_input, 'data')
        messages = event[0]['messages']
        if len(messages) == 0:
            return

        for message in messages:
            event_dict = _json.loads(message)
            if event_dict['eventType'] == 'EVENT_STIMULUSINJECTION':
                params_dict = _json.loads(event_dict['params'])
                self._apply_generators_to_neurons(event_dict['messageID'],
                                                  params_dict,
                                                  event_dict['cells'],
                                                  event_dict['multiple'])

        # Clear messages
        _nest.SetStatus(self._music_steering_input, {'n_messages':0})

    def _apply_generators_to_neurons(self, uuid, parameters, gids, multiple):
        """
        Creates the generator(s), sets status and does the mapping
        between gids and nest neurons.
        """
        generator_name = str(parameters['model'][0])
        del parameters['model'] # Removed to avoid a spurious exception
        generators = _nest.Create(generator_name, len(gids) if multiple else 1)
        try:
            # Coercing numeric parameter types to avoid errors
            defaults = _nest.GetDefaults(generator_name)
            for key, value in parameters.items():
                value_type = type(defaults[key])
                type_name = value_type.__name__
                if type_name == 'int' or type_name == 'float':
                    parameters[key] = value_type(value)
            _nest.SetStatus(generators, parameters)
        except _nest.pynestkernel.NESTError as e:
            if 'DictError' not in str(e):
                print(e)

        self._generator_dict[uuid] = generators

        neurons = []
        for gid in gids:
            neurons.append(self._gid_to_neuron_map[gid])

        self._apply_generators_to_neurons_function(generators, neurons)

    def set_apply_generators_to_neurons_function(self, function):
        """
        Users can have their own function for event for applying generators
        to their circuit.
        """
        self._apply_generators_to_neurons_function = function

    def _setup_music(self):
        """
        Setup music connection.
        """
        _nest.sli_run('statusdict/have_music ::')
        if not _nest.spp():
            print('NEST was not compiled with support for MUSIC, not running.')
            _sys.exit()

        # Setup music spike output.
        self._music_spike_output = _nest.Create('music_event_out_proxy', 1)
        _nest.SetStatus(self._music_spike_output,
                        {'port_name': self._spike_port_name})

        # Connecting neurons to music event channels. In case of the BBP
        # circuit, each channel corresponds to a gid.
        for gid, neuron in self._gid_to_neuron_map.items():
            _nest.Connect([neuron], self._music_spike_output, 'one_to_one',
                          {'music_channel':   gid})

        # Connecting the last music channel (value specified in the
        # configuration file) to a dummy neuron
        dummy = _nest.Create('iaf_neuron', 1)
        _nest.Connect(dummy, self._music_spike_output, 'one_to_one',
                      {'music_channel': self._max_music_channel})

        # Setup music steering input.
        self._music_steering_input = _nest.Create('music_message_in_proxy', 1)
        _nest.SetStatus(self._music_steering_input,
                        {'port_name': self._steering_port_name,
                         'acceptable_latency': 40.0})
