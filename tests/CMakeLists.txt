#
# Copyright (c) 2011-2016, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#


if(NOT BBPTESTDATA_FOUND)
  return()
endif()

set(TEST_LIBRARIES ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
  BBPTestData Monsteer BrionMonsteerSpikeReport Brain)

include(CommonCTest)
