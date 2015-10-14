#
# Copyright (c) 2015, Jafet Villafranca Diaz <jafet.villafrancadiaz@epfl.ch>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if(NOT ZEQ_FOUND OR NOT QT5WIDGETS_FOUND)
  return()
endif()

set(MONSTEERQT_PUBLIC_HEADERS
  qt/SteeringWidget.h)

set(MONSTEERQT_HEADERS
  qt/nestData.h
  qt/types.h
  qt/GeneratorModel.h
  qt/GeneratorPropertiesModel.h
  qt/PropertyEditDelegate.h)

set(MONSTEERQT_MOC_HEADERS
  qt/GeneratorModel.h
  qt/GeneratorPropertiesModel.h
  qt/PropertyEditDelegate.h
  qt/SteeringWidget.h)

set(MONSTEERQT_SOURCES
  qt/nestData.cpp
  qt/GeneratorModel.cpp
  qt/GeneratorPropertiesModel.cpp
  qt/PropertyEditDelegate.cpp
  qt/SteeringWidget.cpp)

set(MONSTEERQT_UI_FORMS qt/gui/SteeringWidget.ui)

set(MONSTEERQT_LINK_LIBRARIES Monsteer Qt5::Widgets zeq zeqHBP)
set(MONSTEERQT_INCLUDE_NAME monsteer/qt)
set(MONSTEERQT_NAMESPACE monsteerqt)

common_library(MonsteerQt)
