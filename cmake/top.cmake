#set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(PROJECT_ROOT ENV{PROJECT_ROOT})
set(ENV{PROJECT_ROOT} ${PROJECT_ROOT})
set(KCONFIG_ROOT ${CMAKE_SOURCE_DIR}/Kconfig)
set(BOARD_DIR ${CMAKE_SOURCE_DIR}/configs)
# set(AUTOCONF_H ${CMAKE_CURRENT_BINARY_DIR}/kconfig/include/generated/autoconf.h)

# # Re-configure (Re-execute all CMakeLists.txt code) when autoconf.h changes
# set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${AUTOCONF_H})

#include(../../cmake/extensions.cmake)
include(../../cmake/python.cmake)
include(../../cmake/kconfig.cmake)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

