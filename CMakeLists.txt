cmake_minimum_required(VERSION 3.12)
project(cervpp)
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DPROJECT_VERSION="0.0.2a")

file(GLOB SRCS
        main.cpp
        src/HttpServer_2/*[.c.cc.cpp]
        src/HttpServer_2/configuration/*[.c.cc.cpp]
        src/HttpServer_2/connection/*[.c.cc.cpp]
        src/HttpServer_2/controller/*[.c.cc.cpp]
        src/HttpServer_2/request/*[.c.cc.cpp]
        src/HttpServer_2/response/*[.c.cc.cpp]
        src/HttpServer_2/thread/*[.c.cc.cpp]
        src/HttpServer_2/staticfile/*[.c.cc.cpp]
        src/HttpServer_2/parser/*[.c.cc.cpp]
        src/HttpServer_2/timer/*[.c.cc.cpp]
)

include_directories(
        include/HttpServer_2
        include/HttpServer_2/configuration
        include/HttpServer_2/connection
        include/HttpServer_2/controller
        include/HttpServer_2/request
        include/HttpServer_2/response
        include/HttpServer_2/thread
        include/HttpServer_2/staticfile
        include/HttpServer_2/parser
        include/HttpServer_2/timer

        ../brutils/include
)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(libs/json)
link_directories(../brutils/lib)
add_executable(cervpp ${SRCS})
target_link_libraries(cervpp
        PRIVATE nlohmann_json::nlohmann_json
        PRIVATE brutils
        PUBLIC pthread)