#
# Copyright (c) 2011-2015, Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#


flatbuffers_generate_c_headers(STEERING_FB steering/stimulus.fbs
                                           steering/playbackState.fbs)

list(APPEND MONSTEER_PUBLIC_HEADERS
   steering/simulator.h
   steering/simulatorPlugin.h)

list(APPEND MONSTEER_HEADERS
  ${STEERING_FB_ZEROEQ_OUTPUTS}
  steering/vocabulary.h
  steering/plugin/nestSimulator.h)

list(APPEND MONSTEER_SOURCES
  steering/simulator.cpp
  steering/vocabulary.cpp
  steering/plugin/nestSimulator.cpp)
