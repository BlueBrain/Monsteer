#
# Copyright (c) 2011-2015, ahmet.bilgili@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

add_subdirectory(bindings)

configure_file(__init__.py
               ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/nrp/__init__.py)

install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/nrp/__init__.py
        DESTINATION ${PYTHON_LIBRARY_SUFFIX}/nrp)

install(FILES Nesteer.py
        DESTINATION ${PYTHON_LIBRARY_SUFFIX}/nrp)

# Enables doing tests without "make install"
configure_file(Nesteer.py ${CMAKE_BINARY_DIR}/lib/nrp/Nesteer.py)

