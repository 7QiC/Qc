#include "qc/log/layout.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <ctime>
#include <functional>

qc::log::Layout::Layout(const std::string& pattern) 
                : m_pattern(pattern) {
    init();
}

std::string qc::log::Layout::layout(Event::ptr event) {
    std::stringstream ss;
    for (auto& it : m_items) {
        it->format(ss, event);
    }
    return ss.str();
}

class TextFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit TextFormatItem(const std::string& str) 
                            : m_str(str) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << m_str;
    }
  private:
    std::string m_str;
};

class NameFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit NameFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getName();
    }
};

class LevelFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit LevelFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << levelToString(event->getLevel());
    }
  private:
    static std::string levelToString(qc::log::Level level) {
        switch (level) {
        #define XX(lvl) \
            case qc::log::Level::lvl: return #lvl;
            XX(DEBUG)
            XX(INFO)
            XX(WARN)
            XX(ERROR)
            XX(FATAL)
        #undef XX
            default:
                return "UNKNOWN";
        }
        return "UNKNOWN";
    }
};

class FileNameFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit FileNameFormatItem(const std::string&) {}
  
    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getFileName();
    }
};

class LineFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit LineFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getLine();
    }
};

class DateTimeFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit DateTimeFormatItem(const std::string& time) 
                                : m_time(time) {
        if (m_time.empty()) {
            m_time = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        struct tm tm_time;
        time_t time = event->getTime();
        localtime_r(&time, &tm_time);
        char buf[64]{0};
        strftime(buf, sizeof(buf), m_time.c_str(), &tm_time);
        os << buf;
    }
  private:
    std::string m_time;
};

class ElapseFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit ElapseFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getElapse();
    }
};

class ThreadIdFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit ThreadIdFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getThreadId();
    }
};

class ThreadNameFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit ThreadNameFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getThreadName();
    }
};

class CoroutineIdFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit CoroutineIdFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getCoroutineId();
    }
};

class ContentFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit ContentFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << event->getContent();
    }
};

class NewLineFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit NewLineFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << std::endl;
    }
};

class PercentFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit PercentFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << '%';
    }
};

class TabFormatItem : public qc::log::Layout::FormatItem {
  public:
    explicit TabFormatItem(const std::string&) {}

    void format(std::ostream& os, qc::log::Event::ptr event) override {
        os << '\t';
    }
};

//符号    子串                   解析方式  注释
//"d"    "%Y-%m-%d %H:%M:%S"    1 		#当前时间
//"T"    ""                     1  		#制表（4空格）
//"t"	 ""						1	    #线程ID
//"T"    ""                     1 		#制表（4空格）
//"F"    ""                     1		#协程ID
//"T"    ""                     1 		#制表（4空格）
//"["    ""                     0		#普通字符
//"p"    ""                     1		#日志级别
//"]"    ""                     0		#普通字符
//"T"    ""                     1  		#制表（4空格）
//"["    ""                     0		#普通字符
//"c"    ""                     1		#日志器名称
//"]"    ""                     0		#普通字符
//"T"    ""                     1 		#制表（4空格）
//"f"    ""                     1		#文件名称
//":"    ""                     0		#普通字符
//"l"    ""                     1		#行号
//"T"    ""                     1 		#制表（4空格）
//"m"    ""                     1		#消息
//"n"    ""                     1 		#换行  
void qc::log::Layout::init() {
    enum STATUS {
        SCAN,
        CREATE
    };

    using ItemFactory = std::function<std::unique_ptr<Layout::FormatItem>(const std::string&)>;
    static std::unordered_map<char, ItemFactory> item_factory_map = {
    #define XX(str, item) \
        {str, [](const std::string& s) { return std::make_unique<item##FormatItem>(s); }}
        XX('o', Text),
        XX('c', Name),
        XX('p', Level),
        XX('f', FileName),
        XX('l', Line),
        XX('d', DateTime),
        XX('r', Elapse),
        XX('T', ThreadId),
        XX('N', ThreadName),
        XX('C', CoroutineId),
        XX('m', Content),
        XX('n', NewLine),
        XX('%', Percent),
        XX('t', Tab)
    #undef XX
    };

    STATUS status = SCAN;
    size_t str_begin = 0, str_end = 0;

    for (size_t i = 0; i < m_pattern.size(); ++i) {
        switch (status) {
            case SCAN: {
                str_begin = i;
                for (str_end = i; str_end < m_pattern.size(); ++str_end) {
                    if (m_pattern[str_end] == '%') {
                        status = CREATE;
                        break;
                    }
                }
                i = str_end;
                m_items.emplace_back(item_factory_map['o'](m_pattern.substr(str_begin, str_end - str_begin)));
                break;
            }
            case CREATE: {
                auto it = item_factory_map.find(m_pattern[i]);
                std::string para_str = "";
                if (m_pattern[i + 1] == '{') {
                    str_begin = i + 2;
                    for (str_end = str_begin + 1; str_end < m_pattern.size(); ++str_end) {
                        if (m_pattern[str_end] == '}') {
                            break;
                        }
                    }
                    if (m_pattern[str_end] != '}') {
                        std::cerr << "Layout::init() error: %" << m_pattern[i] << "[] Para Str error." << std::endl;
                        return;
                    } else {
                        i = str_end;
                        para_str = m_pattern.substr(str_begin, str_end - str_begin);
                    }
                }
                if (it != item_factory_map.end()) {
                    m_items.emplace_back(it->second(para_str));
                } else {
                    m_items.emplace_back(item_factory_map['o']("<error format>"));
                }
                status = SCAN;
                break;
            }
            default:
                std::cerr << "Layout::init() error: " << m_pattern << std::endl;
                return;
        }
    }
}