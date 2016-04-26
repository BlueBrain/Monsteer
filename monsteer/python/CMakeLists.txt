#
# Copyright (c) 2011-2016, ahmet.bilgili@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if( NOT Boost_PYTHON${USE_BOOST_PYTHON_VERSION}_LIBRARY OR
    NOT PYTHONLIBS_FOUND OR NOT PYTHON_EXECUTABLE )

  message(STATUS "No Boost.Python or Python found. Disabling python bindings")
  return()
endif()

configure_file(__init__.py
               ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/monsteer/__init__.py)

install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/monsteer/__init__.py
        DESTINATION ${PYTHON_LIBRARY_SUFFIX}/monsteer)

install(FILES Nesteer.py
        DESTINATION ${PYTHON_LIBRARY_SUFFIX}/monsteer)

# Enables doing tests without "make install"
configure_file(Nesteer.py ${CMAKE_BINARY_DIR}/lib/monsteer/Nesteer.py)

# Python bindings
set(MONSTEER_PYTHON_SOURCE_FILES monsteer.cpp simulator.cpp)

add_library(monsteer_python MODULE ${MONSTEER_PYTHON_SOURCE_FILES})
target_include_directories(monsteer_python PRIVATE
  "$<BUILD_INTERFACE:${PYTHON_INCLUDE_DIRS}>")
add_dependencies(monsteer_python Monsteer)

target_link_libraries(monsteer_python
  Monsteer ${PYTHON_LIBRARIES}
  ${Boost_PYTHON${USE_BOOST_PYTHON_VERSION}_LIBRARY})

set_target_properties(monsteer_python PROPERTIES
  OUTPUT_NAME "_monsteer" PREFIX ""
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/monsteer)

set_property(GLOBAL APPEND PROPERTY MONSTEER_ALL_DEP_TARGETS monsteer_python)
install(TARGETS monsteer_python
  LIBRARY DESTINATION ${PYTHON_LIBRARY_SUFFIX}/monsteer)
