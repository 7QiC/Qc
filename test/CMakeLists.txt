# 测试模块的CMake配置
project(QcTests)

# 添加测试可执行文件
# add_executable(log_test
#     log_test.cpp  # 指定日志测试文件路径
# )

# add_executable(config_test
#     config_test.cpp  # 指定配置测试文件路径
# )

# add_executable(thread_test
#     thread_test.cpp  # 指定线程测试文件路径
# )

# add_executable(macro_test
#     macro_test.cpp  # 指定宏测试文件路径
# )

# add_executable(coroutine_test
#     coroutine_test.cpp
# )

add_executable(scheduler_test
    scheduler_test.cpp
)

# 链接主库和依赖
# target_link_libraries(log_test
#     qc_utils      # 链接主项目库
#     qc_log
# )

# target_link_libraries(config_test
#     qc_utils      # 链接主项目库
#     qc_config
#     qc_log
#     yaml-cpp
# )

# target_link_libraries(thread_test
#     qc_utils      # 链接主项目库
#     qc_config
#     qc_log
#     yaml-cpp
#     qc_thread
# )

# target_link_libraries(macro_test
#     qc_utils
#     qc_log
# )

# target_link_libraries(coroutine_test
#     qc_utils
#     qc_log
#     qc_coroutine
#     qc_config
#     qc_thread
#     qc_scheduler
# )

target_link_libraries(scheduler_test
    qc_utils
    qc_log
    qc_coroutine
    qc_config
    qc_thread
    qc_scheduler
)

# 添加测试（可选）
enable_testing()
# add_test(NAME LogModuleTest COMMAND log_test)
# add_test(NAME ConfigModuleTest COMMAND config_test)
# add_test(NAME ThreadModuleTest COMMAND thread_test)
# add_test(NAME MacroModuleTest COMMAND macro_test)
add_test(NAME CoroutineModuleTest COMMAND coroutine_test)