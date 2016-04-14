#
# Copyright (c) 2011-2015, raphael.dumusc@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

source_group(\\ FILES CMakeLists.txt)

flatbuffers_generate_c_headers(SPIKES_FB spikes.fbs)

set(BRIONMONSTEERSPIKEREPORT_HEADERS
  ${SPIKES_FB_ZEROEQ_OUTPUTS}
  spikeReport.h
  vocabulary.h
)

set(BRIONMONSTEERSPIKEREPORT_SOURCES
  spikeReport.cpp
  vocabulary.cpp
)

set(BRIONMONSTEERSPIKEREPORT_LINK_LIBRARIES Brion Lunchbox ZeroEQ)

common_library(BrionMonsteerSpikeReport)
