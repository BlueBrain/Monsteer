# Copyright (c) 2015, EPFL/Blue Brain Project
#                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if(NOT ZEQ_FOUND)
  message(STATUS "Not building Monsteer GUI due to missing ZeroEQ dependency")
  return()
endif()

if(NOT QT5WIDGETS_FOUND)
  message(STATUS "Not building Monsteer GUI due to missing Qt5 dependency")
  return()
endif()

set(MONSTEERGUI_SOURCES
  monsteerGUI.cpp)

set(MONSTEERGUI_LINK_LIBRARIES Monsteer MonsteerQt Qt5::Widgets zeqHBP)

common_gui_application(monsteerGUI)
