#
# Copyright (c) 2011-2015, Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

flatbuffers_generate_c_headers(STREAMING_FB streaming/spikes.fbs)

list(APPEND MONSTEER_PUBLIC_HEADERS
  streaming/spikeReportReader.h
  streaming/spikeReportWriter.h
  streaming/spikes.h)

list(APPEND MONSTEER_HEADERS
  ${STREAMING_FB_ZEQ_OUTPUTS}
  streaming/detail/spikes.h
  streaming/plugin/spikeReport.h
  streaming/vocabulary.h)

list(APPEND MONSTEER_SOURCES
  streaming/plugin/spikeReport.cpp
  streaming/spikeReportReader.cpp
  streaming/spikeReportWriter.cpp
  streaming/spikes.cpp
  streaming/vocabulary.cpp)
