cmake_minimum_required(VERSION 3.10)

project(mt_server C CXX)

set(${PROJECT_NAME}_SRC
    main.cpp
    server.cpp
    client.cpp
    transceiver.cpp
    server.h
    client.h
    transceiver.h
#    my_functions.h
    )

#set(${PROJECT_NAME}_HEADERS
#    server.h
#    client.h
#    transceiver.h
#    my_functions.h
#    )

source_group(source FILES ${${PROJECT_NAME}_SRC}})
#source_group(headers FILES ${${PROJECT_NAME}_HEADERS}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
target_link_libraries("${PROJECT_NAME}" socket-wrapper)

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
else()
  target_link_libraries("${PROJECT_NAME}" pthread)
endif()

