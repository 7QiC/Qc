#include "qc/log/appender.h"

using namespace qc::log;

Appender::Appender(Level level) 
                    : m_level(level) {}

void Appender::setLevel(Level level) {
    m_level = level;
}

void Appender::setLayout(Layout::ptr layout) {
    m_layout = layout;
}

Layout::ptr Appender::getLayout() const {
    return m_layout;
}

ConsoleAppender::ConsoleAppender(Level level) 
                    : Appender(level) {}

void ConsoleAppender::log(Event::ptr event) {
    if (event->getLevel() >= m_level) {
        std::string str = m_layout->layout(event);
        Mutex::Lock lock(m_mutex);
        std::cout << str;
    }
}

FileAppender::FileAppender(const std::string& filename, Level level) 
                            : m_filename(filename), Appender(level) {
    if (!reopen()) {
        std::cerr << "FileAppender::reopen() error: " << filename << std::endl;
    }
}

bool FileAppender::reopen() {
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename, std::ios::app | std::ios::out);
    return !!m_filestream;
}

void FileAppender::log(Event::ptr event) {
    if (event->getLevel() >= m_level) {
        std::string str = m_layout->layout(event);
        Mutex::Lock lock(m_mutex);
        m_filestream << str;
    }
}