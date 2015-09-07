#
# Copyright (c) 2011-2015, stefan.eilemann@epfl.ch
#
# This file is part of Monsteer <https://github.com/BlueBrain/Monsteer>
#

# General CPack configuration
# Info: http://www.itk.org/Wiki/CMake:Component_Install_With_CPack

set(CPACK_PACKAGE_CONTACT "John Doe <john.doe@epfl.ch>")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_PACKAGE_LICENSE "Proprietary")

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-test-dev")

include(CommonCPack)
