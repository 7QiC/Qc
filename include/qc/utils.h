#ifndef QC_UTILS_H
#define QC_UTILS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <cstdint>
#include <memory>

namespace qc {

template<class T, class X = void, int N = 0>
class Singleton {
  public:
    static T* Instance() {
        static T v;
        return &v;
    }
  private:
};

template<class T, class X = void, int N = 0>
class SingletonPtr {
  public:
    static std::shared_ptr<T> Instance() {
        static std::shared_ptr<T> v(new T);
        return v;
    }
  private:
};

pid_t GetThreadId();

uint32_t GetFiberId();

}

#endif