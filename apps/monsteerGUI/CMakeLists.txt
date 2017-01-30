# Copyright (c) 2015, EPFL/Blue Brain Project
#                     Jafet Villafranca <jafet.villafrancadiaz@epfl.ch>
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

if(NOT TARGET MonsteerQt)
  return()
endif()

set(MONSTEERGUI_SOURCES
  monsteerGUI.cpp)

set(MONSTEERGUI_LINK_LIBRARIES Monsteer MonsteerQt Qt5::Widgets)

common_application(monsteerGUI GUI NOHELP)
