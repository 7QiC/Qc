#ifndef QC_LAYOUT_H
#define QC_LAYOUT_H

#include <iostream>
#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <ctime>
#include "qc/log/event.h"

namespace qc {
namespace log {

class Layout {
    public:
        using ptr = std::shared_ptr<Layout>;
    
        explicit Layout(const std::string& pattern);
    
        // %t  %thread_id  %m%n
        std::string layout(Event::ptr event);
    
        class FormatItem {
        public:
            using ptr = std::unique_ptr<FormatItem>;

            virtual ~FormatItem() = default;
    
            virtual void format(std::ostream& os, Event::ptr event) = 0;
        };
    
    private:
        void init();
        
        std::string m_pattern;                  // 日志格式
        std::vector<FormatItem::ptr> m_items;   // 日志格式化项集合
};

}
}

#endif