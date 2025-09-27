cmake_minimum_required(VERSION 3.29)
project(MatchingEngine)

set(CMAKE_CXX_STANDARD 14)

add_executable(MatchingEngine
    main.cpp
        OrderTypes.h
        Order.h)
