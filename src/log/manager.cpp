#include "qc/log/manager.h"
#include "qc/thread/mutex.h"

qc::log::Manager::Manager() : m_root(nullptr) {
    init();
}

void qc::log::Manager::init() {
    m_pattern = "%d{%Y-%m-%d %H:%M:%S}%t%T%t%N%t%C%t[%p]%t[%c]%t%f:%l%t%m%n";
    m_layout = std::make_shared<Layout>("%d{%Y-%m-%d %H:%M:%S}%t%T%t%N%t%C%t[%p]%t[%c]%t%f:%l%t%m%n");
    m_layouts["%d{%Y-%m-%d %H:%M:%S}%t%T%t%N%t%C%t[%p]%t[%c]%t%f:%l%t%m%n"] = m_layout;
    m_level = Level::UNKNOW;
    Appender::ptr appender = std::make_unique<ConsoleAppender>(Level::UNKNOW);
    appender->setLayout(m_layout);
    m_root = std::make_shared<Logger>("root");
    m_root->addAppender(std::move(appender));
    m_root->setValid(true);
    m_loggers["root"] = m_root;
}

qc::log::Logger::ptr qc::log::Manager::getLogger(const std::string& name) {
    RWMutex::WLock lock(m_mutex);
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }
    Logger::ptr logger = std::make_shared<Logger>(name);
    logger->setValid(false);
    m_loggers[name] = logger;
    return logger;
}

qc::log::Layout::ptr qc::log::Manager::getLayout(const std::string& pattern) {
    RWMutex::WLock lock(m_mutex);
    auto it = m_layouts.find(pattern);
    if (it != m_layouts.end()) {
        return it->second;
    }
    Layout::ptr layout = std::make_shared<Layout>(pattern);
    m_layouts[pattern] = layout;
    return layout;
}