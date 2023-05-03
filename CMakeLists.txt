cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(TicketSystem
        src/bpt.hpp)
