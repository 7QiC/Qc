#include "qc/coroutine/coroutine_module.h"
#include "qc/thread/thread_module.h"
#include "qc/macro.h"

void run() {
    LOG_INFO(ROOT_LOG()) << "111";
    qc::coroutine::Coroutine::getThis()->yield();
    LOG_INFO(ROOT_LOG()) << "222";
    qc::coroutine::Coroutine::getThis()->yield();
    LOG_INFO(ROOT_LOG()) << "333";
}

void test_co() {
    ucontext_t m_ctx;
    if (getcontext(&m_ctx)) {
        QC_ASSERTS(false, "getcontext error");
    }
    void* m_stk = malloc(128 * 1024);
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stk;
    m_ctx.uc_stack.ss_size = 128 * 1024;
    qc::coroutine::Coroutine::setMain(&m_ctx);
    qc::coroutine::Coroutine::ptr main_coro = std::make_shared<qc::coroutine::Coroutine>(run, 128 * 1024);
    LOG_INFO(ROOT_LOG()) << "1";
    main_coro->resume();
    LOG_INFO(ROOT_LOG()) << "2";
    main_coro->resume();
    LOG_INFO(ROOT_LOG()) << "3";
    main_coro->resume();
    free(m_stk);
}

int main() {
    qc::thread::Thread::SetName("main");
    std::vector<qc::thread::Thread::ptr> thrs;

    for (int i = 0; i < 3; ++i) {
        thrs.push_back(std::make_shared<qc::thread::Thread>(test_co, "thread-" + std::to_string(i)));
    }
    for (int i = 0; i < 3; ++i) {
        thrs[i]->join();
    }
    for (int i = 0; i < 3; ++i) {
        thrs[i].reset(new qc::thread::Thread(test_co, "thread-" + std::to_string(i)));
    }
    for (int i = 0; i < 3; ++i) {
        thrs[i]->join();
    }
    return 0;
}