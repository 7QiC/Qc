#include "qc/scheduler/scheduler.h"
#include <random>
#include "qc/thread/thread.h"
#include "qc/macro.h"

thread_local qc::scheduler::Scheduler* qc::scheduler::Scheduler::t_scheduler = nullptr;

qc::scheduler::Scheduler::Scheduler(size_t thread_count, const std::string& name)
    : m_name(name), m_threads_count(thread_count), m_stopped(true) {
    QC_ASSERT(thread_count > 0);
    
    m_thread_tasks.resize(thread_count);
    for (size_t i = 0; i < thread_count; ++i) {
        m_thread_tasks[i] = std::make_unique<qc::thread::LockFreeQueue<Task::ptr>>();
    }
    
    m_global_tasks = std::make_unique<qc::thread::LockFreeQueue<Task::ptr>>();
    m_coro_pool = std::make_unique<qc::thread::LockFreeQueue<qc::coroutine::Coroutine::ptr>>();
}

qc::scheduler::Scheduler::~Scheduler() {
    QC_ASSERT(m_stopped);
}

qc::scheduler::Scheduler* qc::scheduler::Scheduler::GetThis() {
    return t_scheduler;
}

void qc::scheduler::Scheduler::start() {
    if (!m_stopped.exchange(false)) {
        LOG_INFO(ROOT_LOG()) << "scheduler already started";
        return;
    }
    m_threads.resize(m_threads_count);
    m_thread_pid.resize(m_threads_count);
    for (size_t i = 0; i < m_threads_count; ++i) {
        LOG_INFO(ROOT_LOG()) << "run :  " << i;
        m_threads[i] = std::make_shared<qc::thread::Thread>(std::bind(&Scheduler::run, this, i), m_name + "_" + std::to_string(i));
        m_thread_pid[i] = m_threads[i]->getTid();
    }
}

void qc::scheduler::Scheduler::stop() {
    if (m_stopped.exchange(true)) {
        LOG_INFO(ROOT_LOG()) << "scheduler already stopped";
        return;
    }
    m_cv.notifyAll();
    
    for (auto& thread : m_threads) {
        thread->join();
    }
    m_threads.clear();
}

void qc::scheduler::Scheduler::execute(Task::ptr task) {
    if (m_stopped) {
        LOG_ERROR(ROOT_LOG()) << "schedule is stopped";
        return;
    }
    if (task->thread_idx != -1 && task->thread_idx < m_threads_count) {
        m_thread_tasks[task->thread_idx]->enqueue(std::move(task));
    } else {
        m_global_tasks->enqueue(std::move(task));
    }
    m_cv.notifyAll();
}

qc::coroutine::Coroutine::ptr qc::scheduler::Scheduler::getIdleCoroutine(Func cb) {
    if (!m_coro_pool->empty()) {
        auto coro = m_coro_pool->dequeue();
        coro->reset(std::move(cb));
        return coro;
    }
    return std::make_shared<qc::coroutine::Coroutine>(std::move(cb), 0);
}

qc::scheduler::Task::ptr qc::scheduler::Scheduler::pickTask() {
    Task::ptr task;
    size_t thread_idx = qc::thread::Thread::getThreadIdx();
    if (!m_thread_tasks[thread_idx]->empty()) {
        task = m_thread_tasks[thread_idx]->dequeue();
        return task;
    }
    if (!m_global_tasks->empty()) {
        task = m_global_tasks->dequeue();
        return task;
    }
    static std::default_random_engine s_rd(time(0));
    std::uniform_int_distribution<size_t> s_dist(0, m_threads_count  - 1);
    for (size_t i = 0; i < m_threads_count; ++i) {
        size_t steal_idx = (thread_idx + 1 + i) % m_threads_count;
        if (steal_idx == thread_idx) {
            continue;
        }

        if (!m_thread_tasks[steal_idx]->empty()) {
            task = m_thread_tasks[steal_idx]->dequeue();
            return task;
        }
    }
    return nullptr;
}

void qc::scheduler::Scheduler::run(size_t thread_idx) {
    t_scheduler = this;
    qc::thread::Thread::SetThreadIdx(thread_idx);
    QC_ASSERTS(qc::thread::Thread::getThreadIdx() != -1, "thread idx error");

    ucontext_t main_ctx;
    getcontext(&main_ctx);
    qc::coroutine::Coroutine::setMain(&main_ctx);

    while(!m_stopped) {
        auto task = pickTask();
        if (!task) {
            qc::thread::Mutex::Lock lock(m_cv_mutex);
            m_cv.waitFor(m_cv_mutex, std::chrono::milliseconds(100));
            continue;
        }
        
        if (task->cb) {
            auto coro = getIdleCoroutine(std::move(task->cb));
            coro->resume();
            if (coro->status() == qc::coroutine::Status::DEAD || coro->status() == qc::coroutine::Status::ECPT) {
                m_coro_pool->enqueue(coro);
            } else {
                m_thread_tasks[thread_idx]->enqueue(std::make_shared<qc::scheduler::Task>(coro));
            }
        } else {
            auto coro = task->coro;
            coro->resume();
            if (coro->status() == qc::coroutine::Status::DEAD || coro->status() == qc::coroutine::Status::ECPT) {
                m_coro_pool->enqueue(coro);
            } else {
                m_thread_tasks[thread_idx]->enqueue(std::make_shared<qc::scheduler::Task>(coro));
            }
        }
    }

    while(auto task = pickTask()) {
        if (task->cb) {
            auto coro = getIdleCoroutine(std::move(task->cb));
            coro->resume();
            if (coro->status() == qc::coroutine::Status::DEAD || coro->status() == qc::coroutine::Status::ECPT) {
                m_coro_pool->enqueue(coro);
            } else {
                m_thread_tasks[thread_idx]->enqueue(std::make_shared<qc::scheduler::Task>(coro));
            }
        } else {
            auto coro = task->coro;
            coro->resume();
            if (coro->status() == qc::coroutine::Status::DEAD || coro->status() == qc::coroutine::Status::ECPT) {
                m_coro_pool->enqueue(coro);
            } else {
                m_thread_tasks[thread_idx]->enqueue(std::make_shared<qc::scheduler::Task>(coro));
            }
        }
    }
}