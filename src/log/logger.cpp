#include "qc/log/logger.h"
#include "qc/thread/mutex.h"
#include "qc/utils.h"

qc::log::Logger::Logger(const std::string& name)
                : m_name(name){
}

bool qc::log::Logger::log(Event::ptr event) {
    RWMutex::RLock lock(m_mutex);
    if (!m_valid) {
        return false;
    }
    for (auto& appender : m_appenders) {
        appender->log(event);
    }
    return true;
}

// void qc::log::Logger::debug(const char* fmt, std::source_location loc) {
//     if (m_level <= Level::DEBUG) {
//         Event::ptr event = std::make_shared<Event>(loc.file_name(), 
//                                                     loc.line(), 0, 
//                                                     qc::GetThreadId(), 
//                                                     qc::GetFiberId(), 
//                                                     time(nullptr), 
//                                                     Level::DEBUG, 
//                                                     m_name);
//         event->format(fmt);
//         log(event);
//     }
// }

// void qc::log::Logger::info(const char* fmt, std::source_location loc) {
//     if (m_level <= Level::INFO) {
//         Event::ptr event = std::make_shared<Event>(loc.file_name(), 
//                                                     loc.line(), 0, 
//                                                     qc::GetThreadId(), 
//                                                     qc::GetFiberId(), 
//                                                     time(nullptr), 
//                                                     Level::INFO, 
//                                                     m_name);
//         event->format(fmt);
//         log(event);
//     }
// }

// void qc::log::Logger::warn(const char* fmt, std::source_location loc) {
//     if (m_level <= Level::WARN) {
//         Event::ptr event = std::make_shared<Event>(loc.file_name(), 
//                                                     loc.line(), 0, 
//                                                     qc::GetThreadId(), 
//                                                     qc::GetFiberId(), 
//                                                     time(nullptr), 
//                                                     Level::WARN, 
//                                                     m_name);
//         event->format(fmt);
//         log(event);
//     }
// }

// void qc::log::Logger::error(const char* fmt, std::source_location loc) {
//     if (m_level <= Level::ERROR) {
//         Event::ptr event = std::make_shared<Event>(loc.file_name(), 
//                                                     loc.line(), 0, 
//                                                     qc::GetThreadId(), 
//                                                     qc::GetFiberId(), 
//                                                     time(nullptr), 
//                                                     Level::ERROR, 
//                                                     m_name);
//         event->format(fmt);
//         log(event);
//     }
// }

// void qc::log::Logger::fatal(const char* fmt, std::source_location loc) {
//     if (m_level <= Level::FATAL) {
//         Event::ptr event = std::make_shared<Event>(loc.file_name(), 
//                                                     loc.line(), 0, 
//                                                     qc::GetThreadId(), 
//                                                     qc::GetFiberId(), 
//                                                     time(nullptr), 
//                                                     Level::FATAL, 
//                                                     m_name);
//         event->format(fmt);
//         log(event);
//     }
// }

void qc::log::Logger::addAppender(Appender::ptr appender) {
    RWMutex::WLock lock(m_mutex);
    if (!appender->getLayout()) {
        appender->setLayout(m_layout);
    }
    m_appenders.push_back(std::move(appender));
}

void qc::log::Logger::delAppender(Appender::ptr appender) {
    RWMutex::WLock lock(m_mutex);
    m_appenders.remove(appender);
}

void qc::log::Logger::clearAppenders() {
    RWMutex::WLock lock(m_mutex);
    m_appenders.clear();
}

qc::log::Level qc::log::Logger::getLevel() const { 
    RWMutex::RLock lock(m_mutex);
    return m_level; 
}
qc::log::Layout::ptr qc::log::Logger::getLayout() const { 
    RWMutex::RLock lock(m_mutex);
    return m_layout; 
}
void qc::log::Logger::setLevel(Level level) { 
    RWMutex::WLock lock(m_mutex);
    m_level = level; 
}
void qc::log::Logger::setPattern(const std::string& pattern) { 
    RWMutex::WLock lock(m_mutex);
    m_pattern = pattern; 
}
void qc::log::Logger::setLayout(Layout::ptr layout) { 
    RWMutex::WLock lock(m_mutex);
    m_layout = layout; 
}
bool qc::log::Logger::isValid() const { 
    RWMutex::RLock lock(m_mutex);
    return m_valid; 
}
void qc::log::Logger::setValid(bool valid) { 
    RWMutex::WLock lock(m_mutex);
    m_valid = valid; 
}