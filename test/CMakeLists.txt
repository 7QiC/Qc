# 测试模块的CMake配置
project(QcTests)

# 添加测试可执行文件
add_executable(log_test
    log_test.cpp  # 指定日志测试文件路径
)

add_executable(config_test
    config_test.cpp  # 指定配置测试文件路径
)

add_executable(thread_test
    thread_test.cpp  # 指定线程测试文件路径
)

# 链接主库和依赖
target_link_libraries(log_test
    qc_utils      # 链接主项目库
    qc_log
)

target_link_libraries(config_test
    qc_utils      # 链接主项目库
    qc_config
    qc_log
    yaml-cpp
)

target_link_libraries(thread_test
    qc_utils      # 链接主项目库
    qc_config
    qc_log
    yaml-cpp
    qc_thread
)

# 添加测试（可选）
enable_testing()
add_test(NAME LogModuleTest COMMAND log_test)
add_test(NAME ConfigModuleTest COMMAND config_test)
add_test(NAME ThreadModuleTest COMMAND thread_test)