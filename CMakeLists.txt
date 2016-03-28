cmake_minimum_required(VERSION 3.4)
project(ProcessingLogs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(SOURCE_FILES main.cpp Context.hpp StrategyInterface.hpp strategies/UnzipedFileStrategy.cpp strategies/UnzipedFileStrategy.h strategies/GzipedFileStrategy.cpp strategies/GzipedFileStrategy.h strategies/UrlStrategy.cpp strategies/UrlStrategy.h)

find_package(Boost 1.60.0 COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(ProcessingLogs ${SOURCE_FILES})

target_link_libraries(ProcessingLogs ${Boost_LIBRARIES})
