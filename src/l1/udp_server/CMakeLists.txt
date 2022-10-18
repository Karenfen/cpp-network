cmake_minimum_required(VERSION 3.10)

project(udp-server C CXX)

set(${PROJECT_NAME}_SRC
    main.cpp
    udp_server.cpp
    tcpconnection.cpp
    udp_server.h
    tcpconnection.h
    )


source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
target_link_libraries("${PROJECT_NAME}" socket-wrapper)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
endif()

