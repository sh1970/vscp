# - Find libmosquitto
# Find the native libmosquitto includes and libraries
#
#  MOSQUITTO_INCLUDE_DIR  - where to find mosquitto.h, etc.
#  MOSQUITTO_INCLUDE_DIRS - alias for MOSQUITTO_INCLUDE_DIR
#  MOSQUITTO_LIBRARIES    - List of libraries when using libmosquitto.
#  MOSQUITTO_FOUND        - True if libmosquitto found.

if (NOT MOSQUITTO_INCLUDE_DIR)
  find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h
    HINTS
      # macOS / Homebrew
      /opt/homebrew/include
      /usr/local/include
      /opt/homebrew/opt/mosquitto/include
      /usr/local/opt/mosquitto/include
      # Windows / vcpkg (CMAKE_PREFIX_PATH is set by vcpkg toolchain)
      ${CMAKE_PREFIX_PATH}/include
  )
endif()

if (NOT MOSQUITTO_LIBRARY)
  find_library(
    MOSQUITTO_LIBRARY
    NAMES mosquitto
    HINTS
      # macOS / Homebrew
      /opt/homebrew/lib
      /usr/local/lib
      /opt/homebrew/opt/mosquitto/lib
      /usr/local/opt/mosquitto/lib
      # Windows / vcpkg
      ${CMAKE_PREFIX_PATH}/lib
  )
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(
  MOSQUITTO DEFAULT_MSG
  MOSQUITTO_LIBRARY MOSQUITTO_INCLUDE_DIR)

message(STATUS "libmosquitto include dir: ${MOSQUITTO_INCLUDE_DIR}")
message(STATUS "libmosquitto: ${MOSQUITTO_LIBRARY}")
set(MOSQUITTO_LIBRARIES ${MOSQUITTO_LIBRARY})
set(MOSQUITTO_INCLUDE_DIRS ${MOSQUITTO_INCLUDE_DIR})

mark_as_advanced(MOSQUITTO_INCLUDE_DIR MOSQUITTO_LIBRARY)
