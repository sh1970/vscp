cmake_minimum_required(VERSION 3.10)

# Combined test project for guid, vscpdatetime in vscphelper
project(vscp_common_tests LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Building VSCP common unit tests")

# Add subdirectories for individual tests
add_subdirectory(guid)
add_subdirectory(vscpdatetime)
add_subdirectory(vscphelper)
add_subdirectory(mdfparser)

# Custom target to run all tests
# Note: mdfparser must run from its source directory for relative file paths
add_custom_target(test
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/guid/unittest_guid
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscpdatetime/unittest_datetime
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscphelper/unittest_vscphelper
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/mdfparser ${CMAKE_CURRENT_BINARY_DIR}/mdfparser/unittest_mdfparser
    DEPENDS unittest_guid unittest_datetime unittest_vscphelper unittest_mdfparser
    COMMENT "Running all unit tests"
)
