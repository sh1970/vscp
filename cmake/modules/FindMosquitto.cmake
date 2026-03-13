# - Find libmosquitto
# Find the native libmosquitto includes and libraries
#
#  MOSQUITTO_INCLUDE_DIR  - where to find mosquitto.h, etc.
#  MOSQUITTO_INCLUDE_DIRS - alias for MOSQUITTO_INCLUDE_DIR
#  MOSQUITTO_LIBRARIES    - List of libraries when using libmosquitto.
#  MOSQUITTO_FOUND        - True if libmosquitto found.

# Build vcpkg hint paths
if(DEFINED VCPKG_INSTALLED_DIR AND DEFINED VCPKG_TARGET_TRIPLET)
  set(_VCPKG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
  set(_VCPKG_LIB_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
elseif(DEFINED ENV{VCPKG_ROOT} AND DEFINED VCPKG_TARGET_TRIPLET)
  set(_VCPKG_INCLUDE_DIR "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
  set(_VCPKG_LIB_DIR "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
endif()

if (NOT MOSQUITTO_INCLUDE_DIR)
  find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h
    HINTS
      # Windows / vcpkg
      ${_VCPKG_INCLUDE_DIR}
      # macOS / Homebrew
      /opt/homebrew/include
      /usr/local/include
      /opt/homebrew/opt/mosquitto/include
      /usr/local/opt/mosquitto/include
  )
endif()

if (NOT MOSQUITTO_LIBRARY)
  find_library(
    MOSQUITTO_LIBRARY
    NAMES mosquitto
    HINTS
      # Windows / vcpkg
      ${_VCPKG_LIB_DIR}
      # macOS / Homebrew
      /opt/homebrew/lib
      /usr/local/lib
      /opt/homebrew/opt/mosquitto/lib
      /usr/local/opt/mosquitto/lib
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
