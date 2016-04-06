#
# Copyright (c) 2011-2015, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

set(MONSTEER_PUBLIC_HEADERS types.h)
set(MONSTEER_SOURCES)

include_directories(BEFORE SYSTEM ${PROJECT_BINARY_DIR}/monsteer)

include(streaming/files.cmake)
include(steering/files.cmake)

set(MONSTEER_LINK_LIBRARIES PUBLIC Brion Lunchbox ZeroEQ)

common_library(Monsteer)

add_subdirectory(qt)
add_subdirectory(plugin)
add_subdirectory(python)
