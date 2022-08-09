cmake_minimum_required(VERSION 3.10)

# set the project name
project(kvstore VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(sstable)
add_subdirectory(lsmkvstore)

# add the executable
add_executable(kvstore main.cpp)

target_link_libraries(kvstore PUBLIC LSMKVStore)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(kvstore PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/sstable"
        "${PROJECT_SOURCE_DIR}/sstable/command"
        "${PROJECT_SOURCE_DIR}/lib"
        "${PROJECT_SOURCE_DIR}/kvstore"
        )