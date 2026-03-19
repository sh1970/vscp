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
add_subdirectory(tcpiptls)
add_subdirectory(vscp-client-base)
add_subdirectory(vscp-client-canal)
add_subdirectory(vscp-client-mqtt)
add_subdirectory(vscp-client-multicast)
if(LINUX)
  add_subdirectory(vscp-client-socketcan)
endif()
add_subdirectory(vscp-client-udp)
add_subdirectory(vscp-client-ws1)
add_subdirectory(vscp-client-ws2)

# Custom target to run all tests
# Note: mdfparser must run from its source directory for relative file paths

# Build the list of test commands and dependencies
set(TEST_COMMANDS
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/guid/unittest_guid
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscpdatetime/unittest_datetime
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscphelper/unittest_vscphelper
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_CURRENT_SOURCE_DIR}/mdfparser ${CMAKE_CURRENT_BINARY_DIR}/mdfparser/unittest_mdfparser
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tcpiptls/unittest_tcpiptls
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-base/unittest_vscp_client_base
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-canal/unittest_vscp_client_canal
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-mqtt/unittest_vscp_client_mqtt
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-multicast/unittest_vscp_client_multicast
)
set(TEST_DEPENDS
    unittest_guid unittest_datetime unittest_vscphelper unittest_mdfparser
    unittest_tcpiptls unittest_vscp_client_base unittest_vscp_client_canal
    unittest_vscp_client_mqtt unittest_vscp_client_multicast
)

if(LINUX)
    list(APPEND TEST_COMMANDS COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-socketcan/unittest_vscp_client_socketcan)
    list(APPEND TEST_DEPENDS unittest_vscp_client_socketcan)
endif()

list(APPEND TEST_COMMANDS
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-udp/unittest_vscp_client_udp
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-ws1/unittest_vscp_client_ws1
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vscp-client-ws2/unittest_vscp_client_ws2
)
list(APPEND TEST_DEPENDS unittest_vscp_client_udp unittest_vscp_client_ws1 unittest_vscp_client_ws2)

add_custom_target(test
    ${TEST_COMMANDS}
    DEPENDS ${TEST_DEPENDS}
    COMMENT "Running all unit tests"
)
