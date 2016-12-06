#
# Copyright (c) 2011-2015, ahmet.bilgili@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

# SOURCES ---------------------------------------------------------------------

set(NRP_PYTHON_BINDING_FILES
  simulator.cpp
)

# fix for huge python wrapping running out of memory
if(MSVC)
  set_source_files_properties(
    nrp.cpp
    PROPERTIES COMPILE_FLAGS "${WARNING_FLAGS} /bigobj /wd4996")
endif()


# TARGETS ---------------------------------------------------------------------

set(NRP_PYTHON_SOURCE_FILES
  nrp.cpp
  ${NRP_PYTHON_BINDING_FILES})

include_directories(${PYTHON_INCLUDE_DIRS})
add_library(nrp_python MODULE ${NRP_PYTHON_SOURCE_FILES})
add_dependencies(nrp_python Nrp)

target_link_libraries(nrp_python
    Nrp ${PYTHON_LIBRARIES}
     ${Boost_PYTHON${USE_BOOST_PYTHON_VERSION}_LIBRARY})

set_target_properties(nrp_python PROPERTIES
  OUTPUT_NAME "_nrp" PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/nrp)

set_property(GLOBAL APPEND PROPERTY MONSTEER_ALL_DEP_TARGETS nrp_python)

# INSTALL --------------------------------------------------------------------

install(TARGETS nrp_python
  LIBRARY DESTINATION ${PYTHON_LIBRARY_SUFFIX}/nrp)
