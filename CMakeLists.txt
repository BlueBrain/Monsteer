#
# Copyright (c) 2011-2015, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Monsteer)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
  ${CMAKE_SOURCE_DIR}/CMake/common)

include(GitExternal)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "3")
set(VERSION_PATCH "0")
set(VERSION_ABI 3)

set(MONSTEER_ISSUES_URL "https://github.com/BlueBrain/Monsteer/issues")
set(MONSTEER_DESCRIPTION
  "Interactive Supercomputing: Monitoring and Steering for NEST simulations")
set(MONSTEER_MAINTAINER
  "BBP Visualization Team <bbp-open-source@googlegroups.com>")
set(MONSTEER_LICENSE LGPL)
include(Common)

common_package(BBPTestData)
common_package(Boost REQUIRED COMPONENTS filesystem serialization system
                                         python${USE_BOOST_PYTHON_VERSION}
                                         program_options thread
                                         unit_test_framework)
common_package(Brion REQUIRED)
common_package(FlatBuffers REQUIRED)
common_package(Lunchbox REQUIRED)
common_package(MPI)
common_package(music)
common_package(Qt5Widgets)
common_package(PythonInterp)
common_package(PythonLibs)
common_package(vmmlib REQUIRED)
common_package(zeq REQUIRED)
common_package_post()

set(MONSTEER_DEPENDENT_LIBRARIES Lunchbox zeq)

add_subdirectory(monsteer)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(examples)

include(CPackConfig)

set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule)
