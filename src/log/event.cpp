#include "qc/log/event.h"

using namespace qc::log;

Event::Event(const std::string& filename,
             uint32_t line,
             uint32_t elapse,
             uint32_t thread_id,
             uint32_t fiber_id,
             time_t time,
             Level level,
             const std::string& name)
           : m_filename(filename),
             m_line(line),
             m_elapse(elapse),
             m_threadId(thread_id),
             m_fiberId(fiber_id),
             m_time(time),
             m_level(level),
             m_name(name) {}

void Event::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1 && buf != nullptr) {
        m_ss << std::string(buf, len);
        free(buf);
    } else {
        m_ss << "[Format Error]";
    }
}

void Event::format(const char* fmt) {
    m_ss << fmt;
}