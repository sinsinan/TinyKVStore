cmake_minimum_required(VERSION 3.10)

# set the project name
project(TinyKvStore)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

INCLUDE_DIRECTORIES("KVStore.h", "json.h")
#LINK_DIRECTORIES("/opt/homebrew/Cellar/boost/1.79.0_1/lib")
# add the executable
add_executable(TinyKvStore KVSTore.cpp)
