add_subdirectory(command)

add_library(SSTable SSTable.cpp TableMetaInfo.cpp)

target_link_libraries(SSTable PUBLIC Command)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(SSTable PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/sstable/command"
        "${PROJECT_SOURCE_DIR}/sstable"
        "${PROJECT_SOURCE_DIR}/lib"
        )

