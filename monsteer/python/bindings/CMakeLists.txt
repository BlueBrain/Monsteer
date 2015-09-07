#
# Copyright (c) 2011-2015, ahmet.bilgili@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

# SOURCES ---------------------------------------------------------------------

set(MONSTEER_PYTHON_BINDING_FILES
  simulator.cpp
  spikeReportWriter.cpp
  spikeReportReader.cpp
  spikes.cpp
)

# fix for huge python wrapping running out of memory
if(MSVC)
  set_source_files_properties(
    monsteer.cpp
    PROPERTIES COMPILE_FLAGS "${WARNING_FLAGS} /bigobj /wd4996")
endif()


# TARGETS ---------------------------------------------------------------------

set(MONSTEER_PYTHON_SOURCE_FILES
  monsteer.cpp
  ${MONSTEER_PYTHON_BINDING_FILES})

include_directories(${PYTHON_INCLUDE_DIRS})
add_library(monsteer_python MODULE ${MONSTEER_PYTHON_SOURCE_FILES})
add_dependencies(monsteer_python Monsteer)

target_link_libraries(monsteer_python
     Monsteer ${PYTHON_LIBRARIES}
     ${Boost_PYTHON${USE_BOOST_PYTHON_VERSION}_LIBRARY})

set_target_properties(monsteer_python PROPERTIES
  OUTPUT_NAME "_monsteer" PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/monsteer)

set_property(GLOBAL APPEND PROPERTY MONSTEER_ALL_DEP_TARGETS monsteer_python)

# INSTALL --------------------------------------------------------------------

install(TARGETS monsteer_python
  LIBRARY DESTINATION ${PYTHON_LIBRARY_SUFFIX}/monsteer)
