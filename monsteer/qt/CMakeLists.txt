#
# Copyright (c) 2015, Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if(NOT QT5WIDGETS_FOUND OR NOT TARGET Lexis)
  message(STATUS "Not building MonsteerQt due to missing Qt5 or Lexis dependency")
  return()
endif()

set(MONSTEERQT_PUBLIC_HEADERS
  SteeringWidget.h)

set(MONSTEERQT_HEADERS
  GeneratorModel.h
  GeneratorPropertiesModel.h
  nestData.h
  PropertyEditDelegate.h
  SteeringWidget.h
  types.h
)

set(MONSTEERQT_SOURCES
  GeneratorModel.cpp
  GeneratorPropertiesModel.cpp
  nestData.cpp
  PropertyEditDelegate.cpp
  SteeringWidget.cpp
  SteeringWidget.ui
)

set(MONSTEERQT_LINK_LIBRARIES
  PUBLIC Monsteer Qt5::Widgets
  PRIVATE Lexis Lunchbox ZeroEQ
)
set(MONSTEERQT_INCLUDE_NAME monsteer/qt)
set(MONSTEERQT_NAMESPACE monsteerqt)

common_library(MonsteerQt)
