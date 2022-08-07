add_library(Command SetCommand.cpp RmCommand.cpp)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(Command PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/sstable/command"
        "${PROJECT_SOURCE_DIR}/lib"
        )