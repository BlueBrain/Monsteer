#
# Copyright (c) 2015, Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if(NOT ZEROEQ_FOUND OR NOT QT5WIDGETS_FOUND)
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

set(MONSTEERQT_LINK_LIBRARIES Monsteer Qt5::Widgets ZeroEQ ZeroEQHBP)
set(MONSTEERQT_INCLUDE_NAME monsteer/qt)
set(MONSTEERQT_NAMESPACE monsteerqt)

common_library(MonsteerQt)
