#include "qc/log/appender.h"
#include <iostream>
#include "qc/thread/mutex.h"

qc::log::Appender::Appender(Level level) 
                    : m_level(level) {}

void qc::log::Appender::setLevel(Level level) {
    m_level = level;
}

void qc::log::Appender::setLayout(Layout::ptr layout) {
    m_layout = layout;
}

qc::log::Layout::ptr qc::log::Appender::getLayout() const {
    return m_layout;
}

qc::log::ConsoleAppender::ConsoleAppender(Level level) 
                    : Appender(level) {}

void qc::log::ConsoleAppender::log(Event::ptr event) {
    if (event->getLevel() >= m_level) {
        std::string str = m_layout->layout(event);
        Mutex::Lock lock(m_mutex);
        std::cout << str;
    }
}

qc::log::FileAppender::FileAppender(const std::string& filename, Level level) 
                            : m_filename(filename), Appender(level) {
    if (!reopen()) {
        std::cerr << "FileAppender::reopen() error: " << filename << std::endl;
    }
}

bool qc::log::FileAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::app | std::ios::out);
    return !!m_filestream;
}

void qc::log::FileAppender::log(Event::ptr event) {
    if (event->getLevel() >= m_level) {
        std::string str = m_layout->layout(event);
        Mutex::Lock lock(m_mutex);
        m_filestream << str;
    }
}