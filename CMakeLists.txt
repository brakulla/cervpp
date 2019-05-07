cmake_minimum_required(VERSION 3.12)
project(cervpp)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DPROJECT_VERSION="0.0.3a")

file(GLOB SRCS
        main.cpp
        src/*[.c.cc.cpp]
        src/configuration/*[.c.cc.cpp]
        src/tcpserver/*[.c.cc.cpp]
        src/controller/*[.c.cc.cpp]
        src/middleware/*[.c.cc.cpp]
        src/request/*[.c.cc.cpp]
        src/response/*[.c.cc.cpp]
        src/threadpool/*[.c.cc.cpp]
        src/staticfile/*[.c.cc.cpp]
        src/parser/*[.c.cc.cpp]
        src/parser/bodyparser/*[.c.cc.cpp]
        src/timer/*[.c.cc.cpp]

        ../brutils/src/brutils/*[.c.cc.cpp]

        controllers/*[.c.cc.cpp]
)

include_directories(
        include
        include/configuration
        include/tcpserver
        include/controller
        include/middleware
        include/request
        include/response
        include/threadpool
        include/staticfile
        include/parser
        include/parser/bodyparser
        include/timer

        ../brutils/include

        controllers
)

#add_subdirectory(../brutils ../brutils EXCLUDE_FROM_ALL)
link_directories(../brutils/lib)
add_executable(cervpp ${SRCS})
target_link_libraries(cervpp
        PRIVATE brutils
        PUBLIC pthread)