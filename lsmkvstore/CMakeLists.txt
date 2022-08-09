add_library(LSMKVStore LSMKVStore.cpp)

target_link_libraries(LSMKVStore PUBLIC SSTable)

target_include_directories(LSMKVStore PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/sstable/command"
        "${PROJECT_SOURCE_DIR}/sstable"
        "${PROJECT_SOURCE_DIR}/lib"
        "${PROJECT_SOURCE_DIR}/lsmkvstore"
)

