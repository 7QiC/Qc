#include "qc/log/log_module.h"
#include "qc/config/config_module.h"
#include "qc/utils.h"
#include "qc/macro.h"

void test_macro() {
    // LOG_ERROR(ROOT_LOG()) << qc::BacktraceToString(10, 2, "    ");
    QC_ASSERTS(false, 777);
}

int main() {
    test_macro();
    return 0;
}