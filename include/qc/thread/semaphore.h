#ifndef QC_SEMAPHORE_H
#define QC_SEMAPHORE_H 

#include <semaphore.h>
#include <cstdint>
#include <atomic>
#include "qc/utils.h"

namespace qc {
namespace thread {

class Semaphore : public qc::NonCopy {
  public:
    Semaphore(uint32_t count = 0);
    ~Semaphore();

    void wait();
    void notify();
  private:
    sem_t m_sem;
    std::atomic<uint32_t> m_count;
};

}
}

#endif