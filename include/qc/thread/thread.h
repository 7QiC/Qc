#ifndef QC_THREAD_H
#define QC_THREAD_H

#include <functional>
#include <memory>
#include "qc/thread/semaphore.h"
#include "qc/coroutine/coroutine.h"
#include "qc/utils.h"

namespace qc {
namespace thread {

enum class State {
    INIT = 0,
    RUNNING = 1,
    JOINED = 2,
    DETACHED = 3,
    TERMINATED = 4
};

class Thread : private qc::NonCopy {
  public:
    using ptr = std::shared_ptr<Thread>;
    using Func = std::function<void()>;

    Thread(Func cb, const std::string& name);

    ~Thread();

    void join();
    void detach();

    const std::string& getName() const { return m_name; }
    pid_t getTid() const { return m_tid; }
    pthread_t getThreadId() const { return m_thread; }
    State getState() const { return m_state; }

    static Thread* GetThis();
    static void SetThreadIdx(size_t idx);
    static size_t getThreadIdx();
    static void SetName(const std::string& name);
    static const std::string& GetName();
  private:
    static void* run(void* arg);

    Func m_cb;
    std::string m_name;
    pid_t m_tid;
    pthread_t m_thread;
    State m_state;
    Semaphore m_sem;
    static thread_local Thread* t_thread;
    static thread_local std::string t_threadName;
    static thread_local size_t t_threadIdx;
};

}
}

#endif