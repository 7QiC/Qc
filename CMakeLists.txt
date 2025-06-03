cmake_minimum_required(VERSION 3.10)
project(Qc)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件搜索路径
include_directories(include)

# 设置库文件和可执行文件的输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# # 查找 Boost 库
# find_package(Boost REQUIRED)
# include_directories(${Boost_INCLUDE_DIRS})

# 查找 yaml-cpp 库
find_package(yaml-cpp REQUIRED)
include_directories(${YAML_CPP_INCLUDE_DIRS})
# target_link_libraries(your_target_name yaml-cpp::yaml-cpp) # 替换 your_target_name 为你的目标名称

# 添加主库子目录
add_subdirectory(src)

# 添加测试子目录
add_subdirectory(test)