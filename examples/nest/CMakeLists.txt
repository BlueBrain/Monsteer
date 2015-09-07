#
# Copyright (c) 2011-2015, Stefan.Eilemann@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

install(FILES nest2music_proxy.music
              minimal.py
              nest2music_proxy_with_steering.music minimal_steered.py
              send_generator.py
  DESTINATION share/${PROJECT_NAME}/examples/nest COMPONENT dev)
