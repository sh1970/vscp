cmake_minimum_required(VERSION 3.10)

# set the project name
project(test_vscp_client_base LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Build dir: " ${PROJECT_SOURCE_DIR})

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

find_package(OpenSSL REQUIRED)

# TODO Check SSL version and set variable accordingly
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DOPENSSL_API_1_1")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DOPENSSL_API_1_1")

include(FindEXPAT)
find_package(EXPAT REQUIRED)
include_directories(${EXPAT_INCLUDE_DIRS})

# pthreads
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# add the executable
add_executable(unittest_vscp_client_base
    ${PROJECT_SOURCE_DIR}/unittest.cpp
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscp-client-base.h
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscp-client-base.cpp
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/guid.h
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/guid.cpp
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscphelper.h
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscphelper.cpp
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscpdatetime.cpp
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscpdatetime.h
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/vscp.h
    ${PROJECT_SOURCE_DIR}/../../src/common/vscpbase64.h
    ${PROJECT_SOURCE_DIR}/../../src/common/vscpbase64.c
    ${PROJECT_SOURCE_DIR}/../../src/common/vscpmd5.h
    ${PROJECT_SOURCE_DIR}/../../src/common/vscpmd5.c
    ${PROJECT_SOURCE_DIR}/../../src/common/crc.h
    ${PROJECT_SOURCE_DIR}/../../src/common/crc.c
    ${PROJECT_SOURCE_DIR}/../../src/common/crc8.h
    ${PROJECT_SOURCE_DIR}/../../src/common/crc8.c
    ${PROJECT_SOURCE_DIR}/../../src/common/vscp-aes.h
    ${PROJECT_SOURCE_DIR}/../../src/common/vscp-aes.c
)

target_include_directories(unittest_vscp_client_base PUBLIC
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/
    ${PROJECT_SOURCE_DIR}/../../src/common/
    ${PROJECT_SOURCE_DIR}/../../third_party/nlohmann/include
    ${PROJECT_SOURCE_DIR}/../../third_party/spdlog/include
    ${PROJECT_SOURCE_DIR}/../../third_party/mustache/
    ${OPENSSL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
)

target_link_libraries(unittest_vscp_client_base PRIVATE
    m
    dl
    pthread
    OpenSSL::SSL OpenSSL::Crypto
    ${OPENSSL_LIBRARIES}
    ${EXPAT_LIBRARIES}
    gtest
    gtest_main
)
