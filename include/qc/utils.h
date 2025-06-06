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

class NonCopy {
  protected:
    NonCopy() = default;
    ~NonCopy() = default;
    
    NonCopy(const NonCopy&) = delete;
    NonCopy& operator=(const NonCopy&) = delete;
};

class NonMove {
  protected:
    NonMove() = default;
    ~NonMove() = default;

    NonMove(NonMove&&) = delete;
    NonMove& operator=(NonMove&&) = delete;
};

class NonTransfer {
  protected:
    NonTransfer() = default;
    ~NonTransfer() = default;

    NonTransfer(const NonTransfer&) = delete;
    NonTransfer& operator=(const NonTransfer&) = delete;

    NonTransfer(NonTransfer&&) = delete;
    NonTransfer& operator=(NonTransfer&&) = delete;
};

pid_t GetThreadId();

uint32_t GetFiberId();

}

#endif