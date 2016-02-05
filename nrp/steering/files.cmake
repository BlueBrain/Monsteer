#
# Copyright (c) 2011-2015, Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#


flatbuffers_generate_c_headers(NRP_STEERING_FB steering/proxyStatusMsg.fbs
                                           steering/runSimTrigger.fbs
                                           steering/statusRequestMsg.fbs)
                                        

list(APPEND NRP_PUBLIC_HEADERS
   steering/simulator.h
   steering/simulatorPlugin.h)

list(APPEND NRP_HEADERS
    ${NRP_STEERING_FB_ZEQ_OUTPUTS}
  steering/vocabulary.h
  steering/plugin/nestSimulator.h)

list(APPEND NRP_SOURCES
  steering/simulator.cpp
  steering/vocabulary.cpp
  steering/plugin/nestSimulator.cpp)
