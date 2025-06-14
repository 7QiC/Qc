#include "qc/macro.h"
#include "qc/utils.h"
#include "qc/log/log_module.h"
#include "qc/config/config_module.h"
#include "qc/thread/thread_module.h"
#include "qc/coroutine/coroutine_module.h"
#include "qc/scheduler/scheduler_module.h"
#include <thread>

// void test_scheduler() { 
//     static std::atomic<int> cnt = 10;
//     // sleep(1);
//     LOG_INFO(ROOT_LOG()) << "test scheduler";
//     if (--cnt >= 0) {
//         qc::scheduler::Scheduler::GetThis()->schedule(test_scheduler, cnt % 4);
//     sleep(1);
//         qc::scheduler::Scheduler::GetThis()->schedule(test_scheduler, cnt % 4);
//     sleep(1);
//         qc::scheduler::Scheduler::GetThis()->schedule(test_scheduler, cnt % 4);
//     }
//     qc::coroutine::Coroutine::getThis()->yield();
// }

// int main() {
//     qc::scheduler::Scheduler::ptr sc = std::make_shared<qc::scheduler::Scheduler>(4, "test");
//     LOG_INFO(ROOT_LOG()) << "start";
//     sc->start();
//     LOG_INFO(ROOT_LOG()) << "after start";
//     sc->schedule(test_scheduler);
//     sleep(3);
//     sc->stop();
//     return 0;
// }
static std::atomic<int> g_counter{0};
static std::mutex g_mutex;
static std::vector<std::pair<int, int>> g_logs;

// 每个协程执行的任务：多次记录并 yield
void task_fn(int task_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            g_logs.emplace_back(task_id, i);
        }
        g_counter.fetch_add(1, std::memory_order_relaxed);
        LOG_INFO(ROOT_LOG()) << "g_counter++" << g_counter;
        // 手动 yield
        qc::coroutine::Coroutine::getThis()->yield();
    }
    LOG_INFO(ROOT_LOG()) << "task_id: " << task_id << " finished";
}

int main() {
    qc::thread::Thread::SetName("MAIN");
    constexpr int kNumThreads = 10;
    constexpr int kNumTasks = 100;
    constexpr int kIterations = 30;
    constexpr int kExpected = kNumTasks * kIterations;

    // 创建并启动调度器
    auto scheduler = std::make_shared<qc::scheduler::Scheduler>(kNumThreads, "unit_test");
    scheduler->start();

    // 调度多任务
    for (int id = 0; id < kNumTasks; ++id) {
        scheduler->execute(std::make_shared<qc::scheduler::Task>(
            std::bind(task_fn, id, kIterations),
            id % kNumThreads  // 可以测试固定线程调度
        ));
    }

    // 等待任务完成（最多5秒）
    auto start = std::chrono::steady_clock::now();
    while (g_counter.load(std::memory_order_relaxed) < kExpected) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (std::chrono::steady_clock::now() - start > std::chrono::seconds(5)) {
            LOG_INFO(ROOT_LOG()) << "[UnitTest] timeout: only " << g_counter.load() << " steps completed";
            scheduler->stop();
            return 1;
        }
    }

    // 停止调度器
    scheduler->stop();

    // 验证日志长度和顺序
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        assert((int)g_logs.size() == kExpected);
        // 可以根据需求添加更严格的顺序验证
        for (int idx = 0; idx < kExpected; ++idx) {
            int expected_task = g_logs[idx].first;
            int expected_iter = g_logs[idx].second;
            assert(expected_iter < kIterations);
        }
    }

    LOG_INFO(ROOT_LOG()) << "[UnitTest] all " << kExpected << " steps completed successfully";
    return 0;
}
