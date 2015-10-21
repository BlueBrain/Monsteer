#
# Copyright (c) 2011-2015, Juan Hernando <jhernando@fi.upm.es>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

list(APPEND MONSTEER_PUBLIC_HEADERS
  streaming/spikeReportReader.h
  streaming/spikeReportWriter.h
  streaming/spikes.h
)

list(APPEND MONSTEER_HEADERS
  streaming/detail/spikes.h
)

list(APPEND MONSTEER_SOURCES
  streaming/spikeReportReader.cpp
  streaming/spikeReportWriter.cpp
  streaming/spikes.cpp
)
