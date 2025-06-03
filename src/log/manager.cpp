#include "qc/log/manager.h"

using namespace qc::log;

Manager::Manager() {
    init();
}

void Manager::init() {
    m_pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n";
    m_layout = std::make_shared<Layout>("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
    m_layouts["%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"] = m_layout;
    m_level = Level::UNKNOW;
    Appender::ptr appender = std::make_unique<ConsoleAppender>(Level::UNKNOW);
    appender->setLayout(m_layout);
    m_root = std::make_shared<Logger>("root");
    m_root->addAppender(std::move(appender));
    m_root->setValid(true);
    m_loggers["root"] = m_root;
}

Logger::ptr Manager::getLogger(const std::string& name) {
    auto it = m_loggers.find(name);
    if (it != m_loggers.end()) {
        return it->second;
    }
    Logger::ptr logger = std::make_shared<Logger>(name);
    logger->setValid(false);
    m_loggers[name] = logger;
    return logger;
}

Layout::ptr Manager::getLayout(const std::string& pattern) {
    auto it = m_layouts.find(pattern);
    if (it != m_layouts.end()) {
        return it->second;
    }
    Layout::ptr layout = std::make_shared<Layout>(pattern);
    m_layouts[pattern] = layout;
    return layout;
}