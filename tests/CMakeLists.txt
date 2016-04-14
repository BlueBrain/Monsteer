#
# Copyright (c) 2011-2015, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#


if(NOT BBPTESTDATA_FOUND)
  return()
endif()

set(TEST_LIBRARIES
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  BBPTestData
  Monsteer
  BrionMonsteerSpikeReport
)

if(NOT MONSTEER_FOUND)
  set(EXCLUDE_FROM_TESTS monsteer.cpp)
endif()

if(NOT ZEROEQ_FOUND)
  set(EXCLUDE_FROM_TESTS spikeStreamReport.cpp)
endif()

include(CommonCTest)

