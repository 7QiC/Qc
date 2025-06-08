#ifndef QC_MUTEX_H
#define QC_MUTEX_H

#include <pthread.h>
#include "qc/utils.h"
#include "qc/thread/scopedlock.h"

namespace qc {
namespace thread {

class NullMutex {
  public:
    using Lock = ScopedLock<NullMutex>;
    NullMutex() = default;
    ~NullMutex() = default;
    void lock() {}
    void unlock() {}
};

class Mutex : qc::NonTransfer {
  public:
    using Lock = ScopedLock<Mutex>;
    
    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }

    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }
  private:
    pthread_mutex_t m_mutex;
};

class SpinLock : qc::NonTransfer {
  public:
    using Lock = ScopedLock<SpinLock>;

    SpinLock() {
        pthread_spin_init(&m_mutex, 0);
    }

    ~SpinLock() {
        pthread_spin_destroy(&m_mutex);
    }

    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }
  private:
    pthread_spinlock_t m_mutex;
};

class NullRWMutex {
  public:
    using RLock = ReadLock<NullRWMutex>;
    using WLock = WriteLock<NullRWMutex>;
    NullRWMutex() = default;
    ~NullRWMutex() = default;
    void readlock() {}
    void writelock() {}
    void unlock() {}
};

class RWMutex : qc::NonTransfer {
  public:
    using RLock = ReadLock<RWMutex>;
    using WLock = WriteLock<RWMutex>;

    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }

    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void readlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void writelock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }
  private:
    pthread_rwlock_t m_lock;
};

}
}

#endif