cmake_minimum_required(VERSION 3.28)
project(High_Performance_Trade_Engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(.)

add_executable(High_Performance_Trade_Engine
        main.cpp
        order_entry.hpp
        order_book.hpp         # your test driver
        # add other .cpp files here
)


