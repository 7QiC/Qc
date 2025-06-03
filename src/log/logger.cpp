#include "qc/log/logger.h"

using namespace qc::log;

Logger::Logger(const std::string& name)
             : m_name(name){
}

void Logger::log(Event::ptr event) {
    for (auto& appender : m_appenders) {
        appender->log(event);
    }
}

void Logger::debug(const char* fmt, std::source_location loc) {
    if (m_level <= Level::DEBUG) {
        Event::ptr event = std::make_shared<Event>(loc.file_name(), 
                                                    loc.line(), 0, 
                                                    qc::GetThreadId(), 
                                                    qc::GetFiberId(), 
                                                    time(nullptr), 
                                                    Level::DEBUG, 
                                                    m_name);
        event->format(fmt);
        log(event);
    }
}

void Logger::info(const char* fmt, std::source_location loc) {
    if (m_level <= Level::INFO) {
        Event::ptr event = std::make_shared<Event>(loc.file_name(), 
                                                    loc.line(), 0, 
                                                    qc::GetThreadId(), 
                                                    qc::GetFiberId(), 
                                                    time(nullptr), 
                                                    Level::INFO, 
                                                    m_name);
        event->format(fmt);
        log(event);
    }
}

void Logger::warn(const char* fmt, std::source_location loc) {
    if (m_level <= Level::WARN) {
        Event::ptr event = std::make_shared<Event>(loc.file_name(), 
                                                    loc.line(), 0, 
                                                    qc::GetThreadId(), 
                                                    qc::GetFiberId(), 
                                                    time(nullptr), 
                                                    Level::WARN, 
                                                    m_name);
        event->format(fmt);
        log(event);
    }
}

void Logger::error(const char* fmt, std::source_location loc) {
    if (m_level <= Level::ERROR) {
        Event::ptr event = std::make_shared<Event>(loc.file_name(), 
                                                    loc.line(), 0, 
                                                    qc::GetThreadId(), 
                                                    qc::GetFiberId(), 
                                                    time(nullptr), 
                                                    Level::ERROR, 
                                                    m_name);
        event->format(fmt);
        log(event);
    }
}

void Logger::fatal(const char* fmt, std::source_location loc) {
    if (m_level <= Level::FATAL) {
        Event::ptr event = std::make_shared<Event>(loc.file_name(), 
                                                    loc.line(), 0, 
                                                    qc::GetThreadId(), 
                                                    qc::GetFiberId(), 
                                                    time(nullptr), 
                                                    Level::FATAL, 
                                                    m_name);
        event->format(fmt);
        log(event);
    }
}

void Logger::addAppender(Appender::ptr appender) {
    if (!appender->getLayout()) {
        appender->setLayout(m_layout);
    }
    m_appenders.push_back(std::move(appender));
}

void Logger::delAppender(Appender::ptr appender) {
    m_appenders.remove(appender);
}

void Logger::clearAppenders() {
    m_appenders.clear();
}