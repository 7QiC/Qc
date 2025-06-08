#ifndef QC_COROUTINE_H
#define QC_COROUTINE_H

#include <ucontext.h>
#include <functional>
#include <memory>

namespace qc {
namespace coroutine {

enum class Status {
    INIT,
    RADY,
    RUNN,
    SUSP,
    DEAD,
    ECPT
};

class Coroutine {
  public:
    using ptr = std::shared_ptr<Coroutine>;
    using Func = std::function<void()>;

    Coroutine(Func cb, size_t stkSize);
    ~Coroutine();
    void resume();
    void yield();
    void reset(Func cb);
    Status status() const;
    uint64_t getId();
    static void setThis(Coroutine* co);
    static Coroutine* getThis();
    static uint64_t getCount();
    static void setMain(ucontext_t* ctx);
  private:
    static void trampoline(Coroutine* co);

    uint64_t m_id;
    Status m_status;
    Func m_cb;
    void* m_stk;
    size_t m_stkSize;
    ucontext_t m_ctx;
    static thread_local ucontext_t* main_ctx;
    static thread_local uint64_t coro_count;
    static uint64_t coro_id;
    static thread_local Coroutine* cur_coro;
};

}
}

#endif