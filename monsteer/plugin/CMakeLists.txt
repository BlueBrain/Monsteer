#
# Copyright (c) 2011-2015, raphael.dumusc@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

include(zerobufGenerateCxx)
zerobuf_generate_cxx(BRIONMONSTEERSPIKEREPORT ${CMAKE_CURRENT_BINARY_DIR}
  endOfStream.fbs spikes.fbs)

list(APPEND BRIONMONSTEERSPIKEREPORT_HEADERS
  spikeReport.h
)

list(APPEND BRIONMONSTEERSPIKEREPORT_SOURCES
  spikeReport.cpp
)

set(BRIONMONSTEERSPIKEREPORT_LINK_LIBRARIES Brion Lunchbox ZeroBuf ZeroEQ)

common_library(BrionMonsteerSpikeReport)
