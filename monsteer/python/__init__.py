from ._monsteer import *

# Monkey patching Simulator.injectStimulus and Stimulus.injectMultipleStimuli
# to accept a dictionary as input parameter instead of a string
import json as _json
import functools as _functools

Simulator._injectStimulus = Simulator.injectStimulus
Simulator._injectMultipleStimuli = Simulator.injectMultipleStimuli

def _dictToString(dict, json_encoder):
    return (_json.dumps(dict) if json_encoder is None
            else _json.dumps(dict, cls=json_encoder))

def _simulator_inject_stimulus(self, parameters, cell_ids,
                               json_encoder = None):
    """injectStimulus( (Simulator)arg1, (dict)arg2, (cell_ids)arg3, (JSONEncoder)arg4) -> None
    """
    parameters = _dictToString(parameters, json_encoder)
    return Simulator._injectStimulus(self, parameters, cell_ids)

def _simulator_inject_multiple_stimuli(self, parameters, cell_ids,
                                       json_encoder = None):
    """injectMultipleStimuli( (Simulator)arg1, (dict)arg2, (cell_ids)arg3, (JSONEncoder)arg4) -> None
    """
    parameters = _dictToString(parameters, json_encoder)
    return Simulator._injectMultipleStimuli(self, parameters, cell_ids)


_functools.update_wrapper(_simulator_inject_stimulus, Simulator.injectStimulus,
                          assigned=('__name__', '__module__'))
_functools.update_wrapper(_simulator_inject_multiple_stimuli,
                          Simulator.injectMultipleStimuli,
                          assigned=('__name__', '__module__'))

Simulator.injectStimulus = _simulator_inject_stimulus
Simulator.injectMultipleStimuli = _simulator_inject_multiple_stimuli
