#ifndef QC_SCHEDULER_H
#define QC_SCHEDULER_H

#include <memory>
#include <atomic>
#include <functional>
#include <deque>
#include "qc/utils.h"
#include "qc/thread/mutex.h"
#include "qc/thread/lockfreequeue.h"
#include "qc/thread/semaphore.h"
#include "qc/thread/conditionvariable.h"
#include "qc/thread/thread.h"
#include "qc/coroutine/coroutine.h"

namespace qc {
namespace scheduler {

struct Task {
    using ptr = std::shared_ptr<Task>;
    using Func = std::function<void()>;
    using Coro = qc::coroutine::Coroutine;

    Func cb;
    Coro::ptr coro;
    size_t thread_idx;

    Task(Func cb, size_t thread_idx = -1) 
        : cb(std::move(cb)), coro(nullptr), thread_idx(thread_idx) {}
    Task(Coro::ptr coro, size_t thread_idx = -1) 
        : cb(nullptr), coro(coro), thread_idx(thread_idx) {}
};

class Scheduler : public qc::NonCopy {
  public:
    using ptr = std::shared_ptr<Scheduler>;
    using Mutex = qc::thread::Mutex;
    using Func = std::function<void()>;

    Scheduler(size_t thread_count, const std::string& name = "");
    ~Scheduler();

    void start();
    void stop();

    template<class Callable>
    void schedule(Callable&& cb, size_t thread_idx = -1) {
        execute(std::make_shared<Task>(std::forward<Callable>(cb), thread_idx));
    }

    void execute(Task::ptr task);

    static Scheduler* GetThis();

    size_t getThreadCount() const { return m_threads.size(); }
  private:
    void run(size_t thread_idx);
    Task::ptr pickTask();
    qc::coroutine::Coroutine::ptr getIdleCoroutine(Func cb);

    std::string m_name;
    std::atomic<bool> m_stopped;
    size_t m_threads_count;

    qc::thread::LockFreeQueue<qc::coroutine::Coroutine::ptr>::ptr m_coro_pool;
    std::vector<qc::thread::Thread::ptr> m_threads;
    std::vector<qc::thread::LockFreeQueue<Task::ptr>::ptr> m_thread_tasks;
    qc::thread::LockFreeQueue<Task::ptr>::ptr m_global_tasks;
    std::vector<pid_t> m_thread_pid;
    qc::thread::ConditionVariable m_cv;
    qc::thread::Mutex m_cv_mutex;
    static thread_local Scheduler* t_scheduler;
};

}
}

#endif