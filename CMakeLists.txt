cmake_minimum_required(VERSION 3.5)
project(ProcessingLogs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O3")    # -O3 is for optimizing speed of resulted app
set(SOURCE_FILES main.cpp Context.hpp fileHandlingStrategies/ProcessingInterface.hpp fileHandlingStrategies/UnzipedFileStrategy.cpp fileHandlingStrategies/UnzipedFileStrategy.h fileHandlingStrategies/GzipedFileStrategy.cpp fileHandlingStrategies/GzipedFileStrategy.h fileHandlingStrategies/UrlStrategy.cpp fileHandlingStrategies/UrlStrategy.h Log.hpp rowsFilteringStrategies/AllRowsStrategy.hpp Viewer.hpp rowsFilteringStrategies/RowInterface.hpp rowsFilteringStrategies/RequestMethodRowsStrategy.hpp)

find_package(Boost 1.60.0 REQUIRED COMPONENTS filesystem system iostreams)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(ProcessingLogs ${SOURCE_FILES})

target_link_libraries(ProcessingLogs ${Boost_LIBRARIES})
