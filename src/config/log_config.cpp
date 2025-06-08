#include "qc/config/log_config.h"
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include "qc/macro.h"
#include "qc/log/event.h"
#include "qc/log/appender.h"
#include "qc/log/logger.h"
#include "qc/log/manager.h"

qc::ConfigVar<std::unordered_map<std::string, qc::LogConfig>>::ptr g_log_config = 
    qc::Config::Create("logs", std::unordered_map<std::string, qc::LogConfig>{
        // {"root1", LogConfig{
        //     qc::log::Level::INFO, // 设置日志级别
        //     "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n", // 设置日志模式
        //     std::list<AppenderConfig>{{"ConsoleAppender", qc::log::Level::INFO, "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"},
        //                                 {"FileAppender", qc::log::Level::INFO, "%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n", "./system.log"}} // 设置 appenders
        // }}
    }, "this is a log config");

// ConfigVar<std::unordered_map<std::string, LogConfig>>::ptr g_asd = qc::Config::Create("asd", std::unordered_map<std::string, LogConfig>{}, "log config");

qc::ConfigVar<int>::ptr g_coroutine_stack_size = qc::Config::Create("coroutine.stack_size", 128 * 1024, "coroutine stack size");

void setupLogger(const std::string& name, const qc::LogConfig& config) {
    qc::log::Logger::ptr logger = LOG_MGR()->getLogger(name);
    logger->setValid(false);
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
            if (appender_config.type == "ConsoleAppender") {
                appender = std::make_unique<qc::log::ConsoleAppender>(logger->getLevel());
                if (appender_config.level != qc::log::Level::UNKNOW) {
                    appender->setLevel(appender_config.level);
                }
                if (appender_config.pattern.empty()) {
                    appender->setLayout(logger->getLayout());
                } else {
                    appender->setLayout(LOG_MGR()->getLayout(appender_config.pattern));
                }
            } else if (appender_config.type == "FileAppender") {
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
}

qc::Initer::Initer() {
    // for (const auto& [name, config] : g_log_config->getValue()) {
    //     if (name == "root") {
    //         continue;
    //     }
    //     setupLogger(name, config);
    //     LOG_INFO(ROOT_LOG()) << "Logger created: " << name;
    // }
    g_log_config->setListener([](const std::unordered_map<std::string, LogConfig>& old_val,\
                                 const std::unordered_map<std::string, LogConfig>& new_val) {
        LOG_INFO(ROOT_LOG()) << "Log Config Loading";
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
        LOG_INFO(ROOT_LOG()) << "Log Config Loaded";
    });
}

static qc::Initer _init;