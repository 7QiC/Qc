#include "qc/utils.h"

namespace qc {

pid_t GetThreadId() {
    return syscall(SYS_gettid);
}

uint32_t GetFiberId() {
    // Assuming a function to get the current fiber ID exists
    // This is a placeholder as the actual implementation may vary
    // depending on the fiber library used.
    return 0; // Replace with actual fiber ID retrieval logic
}

}