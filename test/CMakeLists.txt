include_directories(${gtest_SOURCE_DIR}/include)

set(TEST_SOURCE_FILES
        basic_check.cpp)

add_executable(MyTests ${TEST_SOURCE_FILES})
target_link_libraries(MyTests gtest_main)