#include "qc/config/log_config.h"

namespace qc {

ConfigVar<std::unordered_map<std::string, LogConfig>>::ptr g_log_config = 
    Config::Create("logs", std::unordered_map<std::string, LogConfig>{
        {"root1", LogConfig{
            qc::log::Level::DEBUG, // 设置日志级别
            "%d{%Y-%m-%d %H:%M:%S} %p %c - %m%n", // 设置日志模式
            std::list<AppenderConfig>{{0, qc::log::Level::INFO, "%p %c - %m%n"}} // 设置 appenders
        }}
    }, "this is a log config");

void setupLogger(const std::string& name, const LogConfig& config) {
    qc::log::Logger::ptr logger = std::make_shared<qc::log::Logger>(name);
    if (config.level != qc::log::Level::UNKNOW) {
        logger->setLevel(config.level);
    } else {
        logger->setLevel(LOG_MGR()->getLevel());
    }

    if (!config.pattern.empty()) {
        logger->setPattern(config.pattern);
        logger->setLayout(LOG_MGR()->getLayout(config.pattern));
    } else {
        logger->setPattern(LOG_MGR()->getPattern());
        logger->setLayout(LOG_MGR()->getLayout());
    }

    logger->clearAppenders();
    if (!config.appenders.empty()) {
        for (const auto& appender_config : config.appenders) {
            qc::log::Appender::ptr appender;
            if (appender_config.type == 0) {
                appender = std::make_unique<qc::log::ConsoleAppender>(logger->getLevel());
                if (appender_config.level != qc::log::Level::UNKNOW) {
                    appender->setLevel(appender_config.level);
                }
                if (appender_config.pattern.empty()) {
                    appender->setLayout(logger->getLayout());
                } else {
                    appender->setLayout(LOG_MGR()->getLayout(appender_config.pattern));
                }
            } else if (appender_config.type == 1) {
                const std::string& filename = appender_config.filename;
                std::filesystem::path file_path(filename);
                // 确保父目录存在（递归创建）
                if (!std::filesystem::exists(file_path.parent_path())) {
                    try {
                        std::filesystem::create_directories(file_path.parent_path());
                        LOG_INFO(ROOT_LOG()) << "Created directory: " << file_path.parent_path();
                    } catch (const std::exception& e) {
                        LOG_ERROR(ROOT_LOG()) << "Failed to create directory for " << filename << ": " << e.what();
                        continue;
                    }
                }

                if (!std::filesystem::exists(file_path)) {
                    try {
                        std::ofstream file(file_path); // 打开文件会自动创建（若不存在）
                        file.close();
                        LOG_INFO(ROOT_LOG()) << "Created file: " << filename;
                    } catch (const std::exception& e) {
                        LOG_ERROR(ROOT_LOG()) << "Failed to create file " << filename << ": " << e.what();
                        continue;
                    }
                }
                appender = std::make_unique<qc::log::FileAppender>(appender_config.filename, logger->getLevel());
                if (appender_config.level != qc::log::Level::UNKNOW) {
                    appender->setLevel(appender_config.level);
                }
                if (appender_config.pattern.empty()) {
                    appender->setLayout(logger->getLayout());
                } else {
                    appender->setLayout(LOG_MGR()->getLayout(appender_config.pattern));
                }
            } else {
                LOG_ERROR(ROOT_LOG()) << "Unknown Appender Type: " << appender_config.type
                                      << " for Logger: " << logger->getName();
                continue;
            }
            logger->addAppender(std::move(appender));
        }
    }
    logger->setValid(true);
    LOG_MGR()->addLogger(logger);
}

LogIniter::LogIniter() {
    for (const auto& [name, config] : g_log_config->getValue()) {
        if (name == "root") {
            continue;
        }
        setupLogger(name, config);
        LOG_INFO(ROOT_LOG()) << "Logger created: " << name;
    }
    g_log_config->setListener([](const std::unordered_map<std::string, LogConfig>& old_val, const std::unordered_map<std::string, LogConfig>& new_val) {
        LOG_INFO(ROOT_LOG()) << "Log Config Changed.";
        for (const auto& [name, config] : new_val) {
            if (name == "root") {
                LOG_ERROR(ROOT_LOG()) << "Cannot modify root logger configuration.";
                continue;
            }
            auto it = old_val.find(name);
            if (it == old_val.end()) {
                LOG_INFO(ROOT_LOG()) << "Logger created: " << name;
            } else {
                if (config == it->second) {
                    continue;
                }
                LOG_INFO(ROOT_LOG()) << "Logger updated: " << name;
            }
            setupLogger(name, config);
        }
        for (const auto& [name, config] : old_val) {
            if (name == "root") {
                continue;
            }
            if (new_val.find(name) == new_val.end()) {
                qc::log::Logger::ptr logger = LOG_MGR()->getLogger(name);
                logger->setValid(false);
                logger->clearAppenders();
                LOG_INFO(ROOT_LOG()) << "Logger invalid: " << name;
            }
        }
    });
}

static LogIniter _log_init;

}