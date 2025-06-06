#ifndef QC_THREAD_H
#define QC_THREAD_H

#include <thread>
#include <memory>
#include <functional>
#include "qc/thread/semaphore.h"
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

    Thread(std::function<void()> cb, const std::string& name);

    ~Thread();

    void join();
    void detach();

    const std::string& getName() const { return m_name; }
    pid_t getTid() const { return m_tid; }
    pthread_t getThreadId() const { return m_thread; }
    State getState() const { return m_state; }

    static Thread* GetThis();
    static void SetName(const std::string& name);
    static std::string GetName();
  private:
    static void* run(void* arg);

    std::function<void()> m_cb;
    std::string m_name;
    pid_t m_tid;
    pthread_t m_thread;
    State m_state;
    Semaphore m_sem;
    static thread_local Thread* t_thread;
    static thread_local std::string t_threadName;
};

}
}

#endif