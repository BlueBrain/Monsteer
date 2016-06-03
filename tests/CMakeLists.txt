#
# Copyright (c) 2011-2016, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#


if(NOT BBPTESTDATA_FOUND)
  return()
endif()

common_find_package(Threads REQUIRED)
set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  BBPTestData Monsteer BrionMonsteerSpikeReport Brain ${CMAKE_THREAD_LIBS_INIT})

include(CommonCTest)
