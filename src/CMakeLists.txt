cmake_minimum_required(VERSION 2.8)
project(ProcessingLogs)

set(CMAKE_BUILD_TYPE Debug)   # Possible types: Release, Debug, MinSizeRel and RelWithDebInfo
set(GENERATE_DEB false) # Switch to build .deb package

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
find_package(Boost 1.55.0 REQUIRED COMPONENTS filesystem system iostreams regex)

include_directories(${Boost_INCLUDE_DIR})

# Note: a target should be already defined using 'add_executable' or 'add_library'
target_link_libraries(ProcessingLogs ${Boost_LIBRARIES})

# For creating DEB archive
# If you want RPM or EXE please read Cmake CPack documentation
if (${GENERATE_DEB})
    install(TARGETS ProcessingLogs DESTINATION /usr/bin)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "efDidymos <mail@domain.com>")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Lightweight adaptation of apache log viewer.")
    set(CPACK_DEBIAN_PACKAGE_SECTION "Text Processing")
    set(CPACK_PACKAGE_VERSION_MAJOR "1")
    set(CPACK_PACKAGE_VERSION_MINOR "0")
    set(CPACK_PACKAGE_VERSION_PATCH "0")
    set(CPACK)
    include(CPack)
endif()
