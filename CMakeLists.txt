#
# Copyright (c) 2011-2017, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(Monsteer VERSION 0.7.0)
set(Monsteer_VERSION_ABI 7)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
                              ${CMAKE_SOURCE_DIR}/CMake/common)
if(NOT EXISTS ${CMAKE_SOURCE_DIR}/CMake/common/Common.cmake)
  message(FATAL_ERROR "CMake/common missing, run: git submodule update --init")
endif()

set(MONSTEER_ISSUES_URL "https://github.com/BlueBrain/Monsteer/issues")
set(MONSTEER_DESCRIPTION
  "Interactive Supercomputing - Monitoring and Steering for NEST simulations")
set(MONSTEER_MAINTAINER
  "BBP Visualization Team <bbp-open-source@googlegroups.com>")
set(MONSTEER_LICENSE LGPL)
include(Common)

common_find_package(BBPTestData)
common_find_package(Boost REQUIRED COMPONENTS filesystem serialization system
                                         python${USE_BOOST_PYTHON_VERSION}
                                         program_options thread
                                         unit_test_framework)
common_find_package(Brion 2 REQUIRED)
common_find_package(Lexis)
common_find_package(Lunchbox REQUIRED)
common_find_package(MPI)
common_find_package(MUSIC SYSTEM)
common_find_package(Qt5Widgets)
common_find_package(PythonInterp)
common_find_package(PythonLibs)
common_find_package(vmmlib REQUIRED)
common_find_package(ZeroBuf REQUIRED)
common_find_package(ZeroEQ REQUIRED)
common_find_package_post()

set(MONSTEER_DEPENDENT_LIBRARIES Lunchbox ZeroBuf)

add_subdirectory(monsteer)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(examples)

include(CPackConfig)

set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)

set(DOXYGEN_MAINPAGE_MD README.md)
set(DOXYGEN_EXTRA_INPUT ${PROJECT_SOURCE_DIR}/README.md)
include(DoxygenRule)
