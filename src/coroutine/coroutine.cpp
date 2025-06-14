#include "qc/coroutine/coroutine.h"
#include <memory.h>
#include "qc/macro.h"

thread_local ucontext_t* qc::coroutine::Coroutine::t_main_ctx = nullptr;
thread_local qc::coroutine::Coroutine* qc::coroutine::Coroutine::t_cur_coro = nullptr;
thread_local uint64_t qc::coroutine::Coroutine::coro_count = 0;
std::atomic<uint64_t> qc::coroutine::Coroutine::coro_id = 0;

int g_coroutine_stk_size = 128 * 1024;

qc::coroutine::Coroutine::Coroutine(Func cb, size_t stackSize) 
    : m_cb(cb), m_status(Status::INIT) {
    ++coro_count;
    m_id = ++coro_id;
    m_stkSize = stackSize ? stackSize : g_coroutine_stk_size;
    m_stk = malloc(m_stkSize);

    memset(&m_ctx, 0, sizeof(ucontext_t));
    if (getcontext(&m_ctx) == -1) {
        QC_ASSERTS(false, "getcontext error");
    }
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stk;
    m_ctx.uc_stack.ss_size = m_stkSize;
    makecontext(&m_ctx, (void (*)())&Coroutine::trampoline, 0);
    m_status = Status::RADY;
    LOG_INFO(ROOT_LOG()) << "coroutine id:" << m_id << " create success";
}

qc::coroutine::Coroutine::~Coroutine() {
    if (m_status != Status::DEAD && m_status != Status::ECPT) {
        QC_ASSERTS(false, "cannot deconstruct status error");
    }
    if (m_stk) {
        free(m_stk);
    }
    --coro_count;
    LOG_INFO(ROOT_LOG()) << "coroutine id:" << m_id << " destroy success";
}

void qc::coroutine::Coroutine::trampoline() {
    try {
        t_cur_coro->m_cb();
        t_cur_coro->m_status = Status::DEAD;
    } catch (std::exception& e) {
        t_cur_coro->m_status = Status::ECPT;
        LOG_ERROR(ROOT_LOG()) << "coroutine id:" << t_cur_coro->getId() << " get exception:" << e.what();
    } catch (...) {
        LOG_ERROR(ROOT_LOG()) << "coroutine id:" << t_cur_coro->getId() << " get unknown exception";
    }
    qc::coroutine::Coroutine* t_co = t_cur_coro;
    setThis(nullptr);
    if (swapcontext(&t_co->m_ctx, t_main_ctx)) {
        QC_ASSERTS(false, "back swapcontext error");
    }
}

void qc::coroutine::Coroutine::resume() {
    if (m_status != Status::RADY && m_status != Status::SUSP) {
        QC_ASSERTS(false, "cannot resume, status error");
    }
    m_status = Status::RUNN;
    setThis(this);
    if (swapcontext(t_main_ctx, &m_ctx)) {
        QC_ASSERTS(false, "resume swapcontext error");
    }
}

void qc::coroutine::Coroutine::yield() {
    if (m_status != Status::RUNN) {
        QC_ASSERTS(false, "cannot yield, status error");
    }
    m_status = Status::SUSP;
    setThis(nullptr);
    if (swapcontext(&m_ctx, t_main_ctx)) {
        QC_ASSERTS(false, "resume swapcontext error");
    }
}

void qc::coroutine::Coroutine::reset(Func cb) {
    if (m_status == Status::RUNN) {
        QC_ASSERTS(false, "cannot reset, status error");
    }
    m_cb = cb;
    m_status = Status::INIT;
    memset(&m_ctx, 0, sizeof(ucontext_t));
    if (getcontext(&m_ctx)) {
        QC_ASSERTS(false, "getcontext error");
    }
    m_ctx.uc_stack.ss_sp = m_stk;
    m_ctx.uc_stack.ss_size = m_stkSize;
    makecontext(&m_ctx, (void (*)())&Coroutine::trampoline, 0);
    m_status = Status::RADY;
    LOG_INFO(ROOT_LOG()) << "coroutine id:" << m_id << " reset success";
}

qc::coroutine::Status qc::coroutine::Coroutine::status() const {
    return m_status;
}

uint64_t qc::coroutine::Coroutine::getId() {
    return m_id;
}

void qc::coroutine::Coroutine::setThis(Coroutine* co) {
    t_cur_coro = co;
}

qc::coroutine::Coroutine* qc::coroutine::Coroutine::getThis() {
    return t_cur_coro;
}

uint64_t qc::coroutine::Coroutine::getCount() {
    return coro_count;
}

void qc::coroutine::Coroutine::setMain(ucontext_t *ctx) {
    t_main_ctx = ctx;
}