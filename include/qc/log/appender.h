#ifndef QC_APPENDER_H
#define QC_APPENDER_H

#include <memory>
#include <iostream>
#include <fstream>
#include "qc/log/layout.h"

namespace qc {
namespace log {

/**
 * @brief 日志输出器基类
 */
class Appender {
  public:
    using ptr = std::unique_ptr<Appender>;
    using Mutex = qc::thread::SpinLock;
    
    explicit Appender(Level level);
    virtual ~Appender() = default;
    
    virtual void log(Event::ptr event) = 0;
    
    void setLevel(Level level);
    void setLayout(Layout::ptr layout);
    Layout::ptr getLayout() const;
    
  protected:
    mutable Mutex m_mutex;
    Level m_level;           // 日志级别
    Layout::ptr m_layout;    // 日志格式化器
};

class ConsoleAppender : public Appender {
  public:
    using ptr = std::unique_ptr<ConsoleAppender>;

    explicit ConsoleAppender(Level level);
        
    virtual void log(Event::ptr event) override;
  private:    
};

class FileAppender : public Appender {
  public:
    using ptr = std::unique_ptr<FileAppender>;

    explicit FileAppender(const std::string& filename, Level level);
    
    virtual void log(Event::ptr event) override;
  private:
    bool reopen();
    
    std::string m_filename;         // 文件名
    std::ofstream m_filestream;     // 文件流
};

}
}

#endif