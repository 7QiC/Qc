#ifndef QC_LOG_CONFIG_H
#define QC_LOG_CONFIG_H

#include <list>
#include <yaml-cpp/yaml.h>
#include "qc/log/event.h"
#include "qc/config/config.h"

namespace qc {

struct AppenderConfig {
    std::string type; // 0: ConsoleAppender, 1: FileAppender, etc.
    log::Level level;
    std::string pattern;
    std::string filename; // For FileAppender

    auto operator<=>(const AppenderConfig&) const = default;
};

struct LogConfig {
    log::Level level;
    std::string pattern;
    std::list<AppenderConfig> appenders;

    auto operator<=>(const LogConfig&) const = default;
};

struct Initer {
    Initer();
};

}

namespace YAML {

template<>
struct convert<qc::AppenderConfig> {
    static Node encode(const qc::AppenderConfig& config) {
        Node node;
        node["type"] = config.type;
        node["level"] = static_cast<int>(config.level);
        node["pattern"] = config.pattern;
        node["filename"] = config.filename;
        return node;
    }

    static bool decode(const Node& node, qc::AppenderConfig& config) {
        if (!node.IsMap() || !node["type"]) {
            return false;
        }
        config.type = node["type"].as<std::string>();
        if (config.type == "FileAppender" && !node["filename"]) {
            return false;
        } 
        if (node["level"]) {
            config.level = static_cast<qc::log::Level>(node["level"].as<int>());
        }
        if (node["pattern"]) {
            config.pattern = node["pattern"].as<std::string>();
        }
        if (node["filename"]) {
            config.filename = node["filename"].as<std::string>();
        }
        return true;
    }
};

template<>
struct convert<qc::LogConfig> {
    static Node encode(const qc::LogConfig& config) {
        Node node;
        node["level"] = static_cast<int>(config.level);
        node["pattern"] = config.pattern;
        node["appenders"] = convert<std::list<qc::AppenderConfig>>::encode(config.appenders);
        return node;
    }

    static bool decode(const Node& node, qc::LogConfig& config) {
        if (node["level"]) {
            config.level = static_cast<qc::log::Level>(node["level"].as<int>());
        }
        if (node["pattern"]) {
            config.pattern = node["pattern"].as<std::string>();
        }
        if (node["appenders"]) {
            config.appenders = node["appenders"].as<std::list<qc::AppenderConfig>>();
        }
        return true;
    }
};

}

#endif