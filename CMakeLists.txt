cmake_minimum_required(VERSION 3.28.3)
project(MatchingEngine)

set(CMAKE_CXX_STANDARD 14)

add_executable(MatchingEngine
    main.cpp
        OrderTypes.h
        Order.h
        OrderTracker/PriceTracker.cpp
        OrderTracker/PriceTracker.h
        OrderTracker/OrderTracker.cpp
        OrderTracker/OrderTracker.h)
