#ifndef QC_LOGGER_H
#define QC_LOGGER_H

#include <list>
#include <source_location>
#include "qc/log/appender.h"
#include "qc/utils.h"
#include "qc/thread/mutex.h"

namespace qc {
namespace log {

class Logger{
  public:
    using ptr = std::shared_ptr<Logger>;
    using RWMutex = qc::thread::RWMutex;

    Logger(const std::string& name);

    bool log(Event::ptr event);

    /**
     * @brief 格式化输出日志
     * @param fmt 日志内容
     */
    void debug(const char* fmt, std::source_location loc = std::source_location::current());
    void info(const char* fmt, std::source_location loc = std::source_location::current());
    void warn(const char* fmt, std::source_location loc = std::source_location::current());
    void error(const char* fmt, std::source_location loc = std::source_location::current());
    void fatal(const char* fmt, std::source_location loc = std::source_location::current());

    void addAppender(Appender::ptr appender);
    void delAppender(Appender::ptr appender);
    void clearAppenders();

    const std::string& getName() const { return m_name; }
    Level getLevel() const;
    Layout::ptr getLayout() const;
    void setLevel(Level level);
    void setPattern(const std::string& pattern);
    void setLayout(Layout::ptr layout);
    bool isValid() const;
    void setValid(bool valid);
  private:
    mutable RWMutex m_mutex;
    std::string m_name;                     // 日志器名称
    Level m_level;                          // 日志有效级别
    std::string m_pattern;                  // 日志格式
    Layout::ptr m_layout;                   // 日志默认格式化器
    std::list<Appender::ptr> m_appenders;   // Appender集合
    bool m_valid;                           // 日志器是否有效
};

}
}

#endif