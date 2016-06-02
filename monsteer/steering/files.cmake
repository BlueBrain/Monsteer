#
# Copyright (c) 2011-2015, Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

include(zerobufGenerateCxx)
zerobuf_generate_cxx(MONSTEER_FBS ${CMAKE_CURRENT_BINARY_DIR}/steering
  steering/playbackState.fbs steering/stimulus.fbs)

list(APPEND MONSTEER_PUBLIC_HEADERS
   ${MONSTEER_FBS_HEADERS}
   steering/simulator.h
   steering/simulatorPlugin.h)

list(APPEND MONSTEER_HEADERS
  steering/plugin/nestSimulator.h)

list(APPEND MONSTEER_SOURCES
  ${MONSTEER_FBS_SOURCES}
  steering/simulator.cpp
  steering/plugin/nestSimulator.cpp)
