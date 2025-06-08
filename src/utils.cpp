#include "qc/utils.h"
#include <sys/types.h>
#include <sys/syscall.h>
#include <execinfo.h>
#include "qc/macro.h"
#include "qc/thread/thread.h"
#include "qc/coroutine/coroutine.h"

pid_t qc::GetThreadId() {
    return syscall(SYS_gettid);
}

const std::string& qc::GetThreadName() {
    return qc::thread::Thread::GetName();
}

uint32_t qc::GetCoroutineId() {
    qc::coroutine::Coroutine* co = qc::coroutine::Coroutine::getThis();
    if (co) {
        return co->getId();
    }
    return 0;
}

void qc::Backtrace(std::vector<std::string>& bt, int size, int skip) {
    void** callstack = (void**)malloc(sizeof(void*) * size);
    if (callstack == nullptr) {
        LOG_ERROR(ROOT_LOG()) << "malloc error";
        return;
    }
    size_t s = ::backtrace(callstack, size);
    char** strs = backtrace_symbols(callstack, s);
    if (strs == nullptr) {
        LOG_ERROR(ROOT_LOG()) << "backtrace error";
    }
    const size_t start = skip < s ? skip : s;
    for (size_t i = skip; i < s; ++i) {
        bt.push_back(strs[i]);
    }
    free(strs);
    if (callstack) {
        free(callstack);
    }
}

std::string qc::BacktraceToString(int size, int skip, const std::string& prefix) { 
    std::vector<std::string> bt;
    Backtrace(bt, size, skip);
    std::stringstream ss;
    for (const auto& i : bt) {
        ss << prefix << i << std::endl; 
    }
    return ss.str();
}