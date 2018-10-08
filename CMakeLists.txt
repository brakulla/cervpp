cmake_minimum_required(VERSION 3.12)
project(cervpp)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)
add_executable(cervpp main.cpp HttpServer/HttpServer.cpp HttpServer/HttpServer.h HttpServer/HttpRequest.cpp HttpServer/HttpRequest.h HttpServer/HttpResponse.cpp HttpServer/HttpResponse.h HttpServer/Tcp/TcpListener.cpp HttpServer/Tcp/TcpListener.h HttpServer/Sender/HttpResponseHandler.cpp HttpServer/Sender/HttpResponseHandler.h HttpServer/Listener/HttpRequestHandler.cpp HttpServer/Listener/HttpRequestHandler.h HttpServer/Tcp/TcpSocket.cpp HttpServer/Tcp/TcpSocket.h HttpServer/HttpCommon.h HttpServer/Controllers/IController.cpp HttpServer/Controllers/IController.h HttpServer/Controllers/DefaultController.cpp HttpServer/Controllers/DefaultController.h)
target_link_libraries(cervpp ${CMAKE_THREAD_LIBS_INIT})