#include "qc/log/event.h"

qc::log::Event::Event(const std::string& filename,
             uint32_t line,
             uint32_t elapse,
             uint32_t thread_id,
             std::string thread_name,
             uint32_t fiber_id,
             time_t time,
             Level level,
             const std::string& name)
           : m_filename(filename),
             m_line(line),
             m_elapse(elapse),
             m_threadId(thread_id),
             m_threadName(thread_name),
             m_coroutineId(fiber_id),
             m_time(time),
             m_level(level),
             m_name(name) {}

void qc::log::Event::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1 && buf != nullptr) {
        m_ss << std::string(buf, len);
        free(buf);
    } else {
        m_ss << "[Format Error]";
    }
}

void qc::log::Event::format(const char* fmt) {
    m_ss << fmt;
}