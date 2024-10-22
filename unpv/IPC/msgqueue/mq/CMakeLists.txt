cmake_minimum_required(VERSION 3.6)

include_directories(include)
add_definitions(-DLOG_USE_COLOR)

add_executable(mqcreatel src/log.c src/mqcreatel.c )

