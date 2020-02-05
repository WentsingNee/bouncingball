cmake_minimum_required(VERSION 3.15)
project(bouncingball)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(acceleration acceleration/main.cpp)
target_link_libraries(acceleration -lnana)
target_link_libraries(acceleration -lXcursor)
target_link_libraries(acceleration -lpthread)

add_executable(billiard billiard/main.cpp)
target_link_libraries(billiard -lnana)
target_link_libraries(billiard -lXcursor)
target_link_libraries(billiard -lpthread)

