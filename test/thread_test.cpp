#include <unistd.h>
#include <assert.h>
#include "qc/utils.h"
#include "qc/log/log_module.h"
#include "qc/config/config_module.h"
#include "qc/thread/thread_module.h"

using namespace qc::thread;

int cnt = 0;

// RWMutex mutex;
Mutex mutex;

void func1() {
    // LOG_INFO(ROOT_LOG()) << "name: " << thread::Thread::GetName()
    //                                 << " this.name: " << thread::Thread::GetThis()->getName()
    //                                 << " tid: " << qc::GetThreadId()
    //                                 << " this.tid: " << thread::Thread::GetThis()->getTid();
    for (int i = 0; i < 100000; ++i) {
        Mutex::Lock lock(mutex);
        cnt++;
    }
}

void func2() { 
    for (int i = 0; i < 1000000; ++i) {
        LOG_INFO(GET_LOGGER("mutex")) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
    }
}

void func3() { 
    for (int i = 0; i < 1000000; ++i) {
        LOG_INFO(GET_LOGGER("mutex")) << "==============================";
    }
}

int main() {
    YAML::Node root = YAML::LoadFile("./conf/mutexlog.yml");
    qc::Config::LoadFromYaml(root);

    LOG_INFO(ROOT_LOG()) << "thread test start";
    std::vector<std::shared_ptr<Thread>> thrs;
    for (int i = 0; i < 5; ++i) {
        Thread::ptr thr1 = std::make_shared<Thread>(&func2, "thread_" + std::to_string(2 * i));
        Thread::ptr thr2 = std::make_shared<Thread>(&func3, "thread_" + std::to_string(2 * i + 1));
        thrs.push_back(thr1);
        thrs.push_back(thr2);
    }
    for (auto& thr : thrs) {
        thr->join();
    }
    // std::cout << cnt << std::endl;
    LOG_INFO(ROOT_LOG()) << "thread test end";
    return 0;
}