#ifndef QC_SCOPED_LOCK_H
#define QC_SCOPED_LOCK_H 

#include "qc/utils.h"

namespace qc {
namespace thread {

template<class T>
class ScopedLock : qc::NonCopy {
  public:
    ScopedLock(T& mutex) 
              : m_mutex(mutex) {
        m_mutex.lock();
    }

    ~ScopedLock() {
        m_mutex.unlock();
    }
  private:
    T& m_mutex;
};

template<class T>
class ReadLock : qc::NonCopy {
  public:
    ReadLock(T& mutex) 
            : m_mutex(mutex) {
        m_mutex.readlock();
    }

    ~ReadLock() {
        m_mutex.unlock();
    }
  private:
    T& m_mutex;
};

template<class T>
class WriteLock : qc::NonCopy {
  public:
    WriteLock(T& mutex) 
              : m_mutex(mutex) {
        m_mutex.writelock();
    }

    ~WriteLock() {
        m_mutex.unlock();
    }
  private:
    T& m_mutex;
};

}
}

#endif