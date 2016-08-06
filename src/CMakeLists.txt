cmake_minimum_required(VERSION 2.8)
project(ProcessingLogs)

set(CMAKE_BUILD_TYPE Debug)   # Possible types: Release, Debug, MinSizeRel and RelWithDebInfo

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
set(SOURCE_FILES
        main.cpp
        Log.hpp
        Viewer.hpp
        handlingFile/IProcessing.hpp
        handlingFile/Unziped.cpp
        handlingFile/Unziped.h
        handlingFile/Gziped.cpp
        handlingFile/Gziped.h
        handlingFile/URL.hpp
        rowsFilteringStrategies/AllRows.hpp
        rowsFilteringStrategies/IRow.hpp
        rowsFilteringStrategies/RequestMethod.hpp
        rowsFilteringStrategies/HTTPCode.hpp
        rowsFilteringStrategies/Date.hpp)

add_executable(ProcessingLogs ${SOURCE_FILES})

# see https://cmake.org/cmake/help/v3.5/module/FindBoost.html
find_package(Boost 1.54.0 REQUIRED COMPONENTS filesystem system iostreams regex)

include_directories(${Boost_INCLUDE_DIR})

# Note: a target should be already defined using 'add_executable' or 'add_library'
target_link_libraries(ProcessingLogs ${Boost_LIBRARIES})
