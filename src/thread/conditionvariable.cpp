#include "qc/thread/conditionvariable.h"

qc::thread::ConditionVariable::ConditionVariable() {
    pthread_cond_init(&m_cond, nullptr);
}

qc::thread::ConditionVariable::~ConditionVariable() {
    pthread_cond_destroy(&m_cond);
}

void qc::thread::ConditionVariable::wait(Mutex& mutex) {
    pthread_cond_wait(&m_cond, mutex.getPhreadMutex());
}

bool qc::thread::ConditionVariable::wait_for(Mutex& mutex, uint64_t nanoseconds) {
    struct timespec abstime;

    abstime.tv_sec = nanoseconds / 1000000000;
    abstime.tv_nsec = nanoseconds % 1000000000;

    if (abstime.tv_nsec >= 1000000000) {
        abstime.tv_sec += 1;
        abstime.tv_nsec -= 1000000000;
    }

    return pthread_cond_timedwait(&m_cond, mutex.getPhreadMutex(), &abstime) == 0;
}

void qc::thread::ConditionVariable::notifyOne() {
    pthread_cond_signal(&m_cond);
}

void qc::thread::ConditionVariable::notifyAll() {
    pthread_cond_broadcast(&m_cond);
}