# - Find libmosquitto
# Find the native libmosquitto includes and libraries
#
#  MOSQUITTO_INCLUDE_DIR  - where to find mosquitto.h, etc.
#  MOSQUITTO_INCLUDE_DIRS - alias for MOSQUITTO_INCLUDE_DIR
#  MOSQUITTO_LIBRARIES    - List of libraries when using libmosquitto.
#  MOSQUITTO_FOUND        - True if libmosquitto found.

if (NOT MOSQUITTO_INCLUDE_DIR)
  find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h
    PATHS
      ${CMAKE_PREFIX_PATH}
      # macOS / Homebrew
      /opt/homebrew
      /usr/local
      /opt/homebrew/opt/mosquitto
      /usr/local/opt/mosquitto
    PATH_SUFFIXES include
  )
endif()

if (NOT MOSQUITTO_LIBRARY)
  find_library(
    MOSQUITTO_LIBRARY
    NAMES mosquitto
    PATHS
      ${CMAKE_PREFIX_PATH}
      # macOS / Homebrew
      /opt/homebrew
      /usr/local
      /opt/homebrew/opt/mosquitto
      /usr/local/opt/mosquitto
    PATH_SUFFIXES lib
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
