#ifndef QC_LOG_MANAGER_H
#define QC_LOG_MANAGER_H

#include <map>
#include "qc/log/logger.h"
#include "qc/log/warp.h"
#include "qc/utils.h"

namespace qc {
namespace log {

class Manager {
  public:
    Manager();

    Logger::ptr getLogger(const std::string& name);
    Layout::ptr getLayout(const std::string& pattern);
    Logger::ptr getRoot() const { return m_root; }
    Layout::ptr getLayout() const { return m_layout; }
    Level getLevel() const { return m_level; }
    std::string getPattern() const { return m_pattern; }
    void addLogger(Logger::ptr logger) { m_loggers[logger->getName()] = logger; }
    void addLayout(const std::string& pattern, Layout::ptr layout) { m_layouts[pattern] = layout; }
  private:
    void init();
    
    std::unordered_map<std::string, Logger::ptr> m_loggers;
    std::unordered_map<std::string, Layout::ptr> m_layouts;
    std::string m_pattern;
    Logger::ptr m_root;
    Level m_level;
    Layout::ptr m_layout;
};

using LoggerMgr = qc::Singleton<Manager>;

}
}

#endif