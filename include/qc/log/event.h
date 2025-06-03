#ifndef QC_LOG_EVENT_H
#define QC_LOG_EVENT_H

#include <string>
#include <cstdint>
#include <memory>
#include <sstream>

namespace qc {
namespace log {

// 日志级别
enum class Level {
    UNKNOW = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

// 日志事件
class Event {
  public:
    using ptr = std::shared_ptr<Event>;
    
    Event(const std::string& filename,
          uint32_t line,
          uint32_t elapse,
          uint32_t thread_id,
          uint32_t fiber_id,
          time_t time,
          Level level,
          const std::string& name);
          
    const std::string& getFileName() const { return m_filename; }
    Level getLevel() const { return m_level; }
    uint32_t getLine() const { return m_line; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint32_t getElapse() const { return m_elapse; }
    time_t getTime() const { return m_time; }
    const std::string& getName() const { return m_name; }
    std::string getContent() const { return m_ss.str(); }
    std::stringstream& getSS() { return m_ss; }
    void appendName(const std::string& str) { m_name += str;}
    
    void format(const char* fmt, va_list al);
    void format(const char* fmt);
  private:
    std::string m_filename;   // 文件名
    Level m_level;            // 日志级别
    uint32_t m_line;          // 行号
    uint32_t m_threadId;      // 线程ID
    uint32_t m_fiberId;       // 协程ID
    uint32_t m_elapse;        // 程序启动至今毫秒数
    time_t m_time;            // 时间戳 
    std::string m_name;       // 日志器名称
    std::stringstream m_ss;   // 日志内容
};

}
}

#endif