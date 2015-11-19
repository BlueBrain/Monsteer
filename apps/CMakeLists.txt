#
# Copyright (c) 2011-2015, Daniel.Nachbaur@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(monsteerGUI)

set(MONSTEER_SPIKE_RECEIVER_SOURCES monsteer_spike_receiver.cpp)
set(MONSTEER_SPIKE_RECEIVER_LINK_LIBRARIES Brion)
common_application(monsteer_spike_receiver)

if(NOT MUSIC_FOUND OR NOT MPI_FOUND)
  return()
endif()

# This macro adds the music library to a target link variables taking into
# account that music can be built with autoconf and CMake/common.
macro(add_music DESTINATION)
  if(TARGET music)
     list(APPEND ${DESTINATION}_LINK_LIBRARIES music)
  else()
     list(APPEND ${DESTINATION}_LINK_LIBRARIES ${MUSIC_LIBRARY})
  endif()
endmacro()

set(MUSIC_SPIKE_WRITER_SOURCES music_spike_writer.cpp)
set(MUSIC_SPIKE_WRITER_LINK_LIBRARIES Lunchbox ${MPI_CXX_LIBRARIES})
add_music(MUSIC_SPIKE_WRITER)
common_application(music_spike_writer)

set(MUSIC_PROXY_SOURCES music_proxy.cpp)
set(MUSIC_PROXY_LINK_LIBRARIES Monsteer ${MPI_CXX_LIBRARIES}
                               ${Boost_PROGRAM_OPTIONS_LIBRARY})
add_music(MUSIC_PROXY)
common_application(music_proxy)
