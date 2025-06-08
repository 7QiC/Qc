add_library(qc_utils SHARED utils.cpp)

add_subdirectory(log)

add_subdirectory(config)

add_subdirectory(thread)

add_subdirectory(coroutine)