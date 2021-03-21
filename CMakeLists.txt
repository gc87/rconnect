cmake_minimum_required(VERSION 3.5.1)
project(rconnect)

set(CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${CMAKE_DIR})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

#c-algorithms
#https://fragglet.github.io/c-algorithms/doc/
file(GLOB CAlgorithms_SRC "external_libs/c-algorithms/src/*.c")
include_directories("external_libs/c-algorithms/src/")

#iniparser
file(GLOB IniParser_SRC "external_libs/iniparser/src/*.c")
include_directories("external_libs/iniparser/src/")

include_directories("/usr/local/include/python3.9")

#server
set(src rconnect.c zmq_server.c)
set(src ${src} ${CAlgorithms_SRC})
set(src ${src} ${IniParser_SRC})
add_executable(rconnect ${src})
target_link_libraries(rconnect protobuf-c m pthread)

#client
set(src rnode.c zmq_client.c)
set(src ${src} ${CAlgorithms_SRC})
set(src ${src} ${IniParser_SRC})
add_executable(rnode ${src})
target_link_libraries(rnode python3.9 protobuf-c m dl pthread util)

#cli
set(src rcli.c zmq_client.c)
set(src ${src} ${CAlgorithms_SRC})
set(src ${src} ${IniParser_SRC})
add_executable(rcli ${src})
target_link_libraries(rcli protobuf-c pthread)
