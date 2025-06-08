#include "qc/thread/thread.h"
#include <pthread.h>
#include <memory>

thread_local qc::thread::Thread* qc::thread::Thread::t_thread = nullptr;
thread_local std::string qc::thread::Thread::t_threadName = "NONE";

qc::thread::Thread* qc::thread::Thread::GetThis() {
    return t_thread;
}

const std::string& qc::thread::Thread::GetName() {
    return t_threadName;
}

void qc::thread::Thread::SetName(const std::string& name) {
    if (name.empty()) {
        return;
    }
    if (t_thread) {
        t_thread->m_name = name;
    }
    t_threadName = name;
}

qc::thread::Thread::Thread(Func cb, const std::string& name) 
                : m_cb(cb), m_name(name), m_state(State::INIT) {
    if (name.empty()) {
        m_name = "NONE";
    }
    m_state = State::RUNNING;
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        throw std::logic_error("pthread_create error");
    }
    m_sem.wait();
}

qc::thread::Thread::~Thread() {
    switch (m_state) {
        case State::RUNNING: {
            int rt = pthread_detach(m_thread);
            break;
        }
        case State::JOINED: {
            break;
        }
        case State::DETACHED: {
            break;
        }
        case State::TERMINATED: {
            int rt = pthread_join(m_thread, nullptr);
            break;
        }
    }
}

void* qc::thread::Thread::run(void* arg) {
    t_thread = static_cast<Thread*>(arg);
    t_threadName = t_thread->m_name;
    t_thread->m_tid = qc::GetThreadId();
    pthread_setname_np(pthread_self(), t_threadName.substr(0, 15).c_str());

    Func cb;
    cb.swap(t_thread->m_cb);
    t_thread->m_sem.notify();
    try {
        cb();
    } catch (const std::exception& e) {
        throw std::logic_error(e.what());
    }
    t_thread->m_state = State::TERMINATED;
    return nullptr;
}

void qc::thread::Thread::join() {
    if (m_state == State::JOINED) {
        return;
    }
    if (m_state == State::DETACHED) {
        throw std::logic_error("Cannot join a detached thread");
    }
    int rt = pthread_join(m_thread, nullptr);
    if (rt) {
        throw std::logic_error("pthread_join error");
    }
    m_state = State::JOINED;
}

void qc::thread::Thread::detach() {
    if (m_state == State::DETACHED) {
        return;
    }
    if (m_state == State::JOINED) {
        throw std::logic_error("Cannot detach a joined thread");
    }
    if (m_state == State::TERMINATED) {
        throw std::logic_error("Cannot detach a terminated thread");
    }
    int rt = pthread_detach(m_thread);
    if (rt) {
        throw std::logic_error("pthread_detach error");
    }
    m_state = State::DETACHED;
}