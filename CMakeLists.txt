cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(code
        src/utility/bpt.hpp
        src/utility/vector.hpp
        src/utility/pair.hpp
        src/utility/exceptions.hpp
        src/head-files/tokenScanner.hpp
        src/head-files/user.hpp
        src/head-files/train.hpp
        src/head-files/releasedTrain.hpp
        src/head-files/transaction.hpp
        src/head-files/waitingList.hpp
        src/main.cpp src/head-files/loginList.hpp src/utility/map.hpp src/utility/tool.hpp)
