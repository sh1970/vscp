cmake_minimum_required(VERSION 3.10)

# set the project name
project(test_vscpdatetime LANGUAGES CXX C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Build dir: " ${PROJECT_SOURCE_DIR})

find_package(OpenSSL REQUIRED)

# TODO Check SSL version and set variable accordingly
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -DOPENSSL_API_1_1")
set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -DOPENSSL_API_1_1")

include(FindEXPAT)
find_package(EXPAT REQUIRED)
include_directories(${EXPAT_INCLUDE_DIRS})

# add the executable
add_executable(unittest_datetime 
    unittest.cpp
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

target_include_directories(unittest_datetime PUBLIC 
    ${PROJECT_SOURCE_DIR}/../../src/vscp/common/
    ${PROJECT_SOURCE_DIR}/../../src/common/
    ${PROJECT_SOURCE_DIR}/../../third_party/nlohmann/include
    ${PROJECT_SOURCE_DIR}/../../third_party/spdlog/include
    ${OPENSSL_INCLUDE_DIR}
    ${EXPAT_INCLUDE_DIRS}
)

target_link_libraries(unittest_datetime PRIVATE m
    dl
    pthread
    OpenSSL::SSL OpenSSL::Crypto
    ${OPENSSL_LIBRARIES}
    ${EXPAT_LIBRARIES}
    gtest
    gtest_main
)
