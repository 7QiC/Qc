#include "qc/thread/semaphore.h"

using namespace qc::thread;

Semaphore::Semaphore(uint32_t count) {
    if (count < 0) {
        throw std::invalid_argument("semaphore count must > 0");
    }
    if (sem_init(&m_sem, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}

Semaphore::~Semaphore() {
    sem_destroy(&m_sem);
}

void Semaphore::wait() {
    if (sem_wait(&m_sem)) {
        throw std::logic_error("sem_wait error");
    }
}

void Semaphore::notify() {
    if (sem_post(&m_sem)) {
        throw std::logic_error("sem_post error");
    }
}