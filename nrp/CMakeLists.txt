#
# Copyright (c) 2011-2015, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

source_group(\\ FILES CMakeLists.txt)

set(NRP_PUBLIC_HEADERS)
set(NRP_SOURCES)

include_directories(BEFORE SYSTEM ${PROJECT_BINARY_DIR}/nrp)

#include(streaming/files.cmake)
include(steering/files.cmake)
#include(qt/files.cmake)

set(NRP_LINK_LIBRARIES PUBLIC Brion Lunchbox zeq)

common_library(Nrp)

#add_subdirectory(plugin)
add_subdirectory(python)
