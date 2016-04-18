cmake_minimum_required(VERSION 3.5)
project(ProcessingLogs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3 -pthread")    # -O3 is for optimizing speed of resulted app
set(SOURCE_FILES
        main.cpp
        Log.hpp
        Viewer.hpp
        handlingFile/ProcessingInterface.hpp
        handlingFile/Unziped.cpp
        handlingFile/Unziped.h
        handlingFile/Gziped.cpp
        handlingFile/Gziped.h
        handlingFile/URL.cpp
        handlingFile/URL.h
        rowsFilteringStrategies/AllRows.hpp
        rowsFilteringStrategies/RowInterface.hpp
        rowsFilteringStrategies/RequestMethod.hpp
        rowsFilteringStrategies/HTTPCode.hpp
        rowsFilteringStrategies/Date.hpp)

find_package(Boost 1.60.0 REQUIRED COMPONENTS filesystem system iostreams regex)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(ProcessingLogs ${SOURCE_FILES})

target_link_libraries(ProcessingLogs ${Boost_LIBRARIES})
