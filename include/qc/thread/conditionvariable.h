#ifndef QC_CONDITIONVARIABLE_H
#define QC_CONDITIONVAR_H

#include <pthread.h>
#include <chrono>
#include "qc/utils.h"
#include "qc/thread/mutex.h"

namespace qc {
namespace thread {

class ConditionVariable : qc::NonCopy {
  public:
    ConditionVariable();
    ~ConditionVariable();

    void wait(Mutex& mutex);
    template<class Rep, class Period>
    bool waitFor(Mutex& mutex, const std::chrono::duration<Rep, Period>& timeout_duration) {
        return wait_for(mutex, std::chrono::nanoseconds(timeout_duration).count());
    }

    void notifyOne();
    void notifyAll();
  private:
    bool wait_for(Mutex& mutex, uint64_t timeout_nanoseconds);
    pthread_cond_t m_cond;
};

}
}

#endif