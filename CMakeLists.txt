cmake_minimum_required(VERSION 3.10)

# set the project name
project(TinyKvStore VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(sstable)

# add the executable
add_executable(TinyKvStore main.cpp)

target_link_libraries(TinyKvStore PUBLIC SSTable)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(TinyKvStore PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/sstable"
        "${PROJECT_SOURCE_DIR}/sstable/command"
        "${PROJECT_SOURCE_DIR}/lib"
        )