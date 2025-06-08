#ifndef QC_UTILS_H
#define QC_UTILS_H

#include <cstdint>
#include <string>
#include <vector>

namespace qc {

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

const std::string& GetThreadName();

uint32_t GetCoroutineId();

void Backtrace(std::vector<std::string>& bt, int size, int skip);

std::string BacktraceToString(int size, int skip, const std::string& prefix);

}

#endif