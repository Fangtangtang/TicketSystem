cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(code
        #utility
        src/utility/bpt.hpp
        src/utility/vector.hpp
        src/utility/pair.hpp
        src/utility/exceptions.hpp
        src/utility/map.hpp
        src/utility/tool.hpp
        src/utility/file_manager.hpp
        src/utility/BPlusTree.hpp

        #headFile
        src/head-files/tokenScanner.hpp
        src/head-files/user.hpp
        src/head-files/train.hpp
        src/head-files/ticket.hpp
        src/head-files/transaction.hpp
        src/head-files/waitingList.hpp
        src/head-files/loginList.hpp
        src/head-files/parameter.hpp

        src/main.cpp

        #try
#        my_data/tryTime.cpp
        #my_data/tryTransaction.cpp
        #my_data/tryFileManager.cpp
        #my_data/tryParameter.cpp
#        my_data/User/try1.cpp
#        my_data/tryScanner.cpp
#        my_data/Ticket/try1.cpp
#        my_data/trySort.cpp
        )


#set(test_prefix "${PROJECT_SOURCE_DIR}/testcases")
#file(GLOB tests RELATIVE "${test_prefix}" testcases/*)
#
#foreach (test-name IN ITEMS ${tests})
#    if (NOT IS_DIRECTORY "${test_prefix}/${test-name}")
#        continue()
#    endif ()
#    add_test(NAME "${test-name}"
#            COMMAND bash ../run-test.sh "${test_prefix}" "${test-name}" "$<TARGET_FILE:code>"
#            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/run
##            TIMEOUT 30
#            )
#    set_tests_properties("${test-name}" PROPERTIES TIMEOUT 1000000)
#endforeach ()
#
#enable_testing()
