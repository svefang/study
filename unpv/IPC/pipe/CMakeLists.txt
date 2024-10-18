cmake_minimum_required(VERSION 3.6)

include_directories(include)
add_definitions(-DLOG_USE_COLOR)

add_executable(client src/client.c src/rw.c)
add_executable(server src/server.c src/log.c src/rw.c)

