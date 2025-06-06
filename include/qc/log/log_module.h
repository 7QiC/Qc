#ifndef QC_LOG_MODULE_H
#define QC_LOG_MODULE_H

#include "event.h"
#include "layout.h"
#include "appender.h"
#include "logger.h"
#include "warp.h"
#include "manager.h"

#define LOG_MGR() qc::log::LoggerMgr::Instance()
#define ROOT_LOG() qc::log::LoggerMgr::Instance()->getRoot()
#define GET_LOGGER(name) qc::log::LoggerMgr::Instance()->getLogger(name)

#define LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        qc::log::Warp(logger, std::make_shared<qc::log::Event>(__FILE__, __LINE__, 0, \
            qc::GetThreadId(), qc::GetFiberId(), \
                time(nullptr), level, logger->getName()))

#define LOG_DEBUG(logger) LOG_LEVEL(logger, qc::log::Level::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, qc::log::Level::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, qc::log::Level::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, qc::log::Level::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, qc::log::Level::FATAL)

#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level) \
        qc::log::Warp(logger, std::make_shared<qc::log::Event>(__FILE__, __LINE__, 0, \
            qc::GetThreadId(), qc::GetFiberId(), \
                time(nullptr), level, logger->getName())).format(fmt, ##__VA_ARGS__);

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::DEBUG, fmt, ##__VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::INFO, fmt, ##__VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::WARN, fmt, ##__VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::ERROR, fmt, ##__VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::FATAL, fmt, ##__VA_ARGS__)

#endif