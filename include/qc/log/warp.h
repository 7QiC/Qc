#ifndef QC_WARP_H
#define QC_WARP_H

#include <sstream>
#include "qc/log/event.h"
#include "qc/log/logger.h"

namespace qc {
namespace log {

class Warp {
  public:
    Warp(Logger::ptr logger, Event::ptr event);

    ~Warp();

    void format(const char* fmt, ...);

    template<typename T>
    std::stringstream& operator<<(T value) {
        std::stringstream& ss = m_event->getSS();
        ss << value;
        return ss;
    }
  private:
    Event::ptr m_event;     // 日志事件
    Logger::ptr m_logger;   // 日志器
    static Logger::ptr m_root;     // 根日志器
};

}
}

#endif