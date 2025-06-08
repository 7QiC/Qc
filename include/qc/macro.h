#ifndef QC_MACRO_H
#define QC_MACRO_H 

#include <assert.h>
#include "qc/log/event.h"
#include "qc/log/logger.h"
#include "qc/log/warp.h"
#include "qc/log/manager.h"

#define LOG_MGR() qc::log::Manager::GetInstance()
#define ROOT_LOG() qc::log::Manager::GetInstance()->getRoot()
#define GET_LOGGER(name) qc::log::Manager::GetInstance()->getLogger(name)

#define LOG_LEVEL(logger, level) \
    if (logger->getLevel() <= level) \
        qc::log::Warp(logger, std::make_shared<qc::log::Event>(__FILE__, __LINE__, 0, \
            qc::GetThreadId(), qc::GetThreadName(), qc::GetCoroutineId(), \
                time(nullptr), level, logger->getName()))

#define LOG_DEBUG(logger) LOG_LEVEL(logger, qc::log::Level::DEBUG)
#define LOG_INFO(logger) LOG_LEVEL(logger, qc::log::Level::INFO)
#define LOG_WARN(logger) LOG_LEVEL(logger, qc::log::Level::WARN)
#define LOG_ERROR(logger) LOG_LEVEL(logger, qc::log::Level::ERROR)
#define LOG_FATAL(logger) LOG_LEVEL(logger, qc::log::Level::FATAL)

#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if (logger->getLevel() <= level) \
        qc::log::Warp(logger, std::make_shared<qc::log::Event>(__FILE__, __LINE__, 0, \
            qc::GetThreadId(), qc::GetThreadName(), qc::GetCoroutineId(), \
                time(nullptr), level, logger->getName())).format(fmt, ##__VA_ARGS__);

#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::DEBUG, fmt, ##__VA_ARGS__)
#define LOG_FMT_INFO(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::INFO, fmt, ##__VA_ARGS__)
#define LOG_FMT_WARN(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::WARN, fmt, ##__VA_ARGS__)
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::ERROR, fmt, ##__VA_ARGS__)
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, qc::log::Level::FATAL, fmt, ##__VA_ARGS__)

#define QC_ASSERT(x) \
            if (!(x)) { \
                LOG_ERROR(ROOT_LOG()) << "ASSERTION: " << #x "\n" \
                << "Backtrace: \n" \
                << qc::BacktraceToString(100, 2, "    "); \
                assert(x); \
            }

#define QC_ASSERTS(x, w) \
            if (!(x)) { \
                LOG_ERROR(ROOT_LOG()) << "ASSERTION: " << #x "\t" #w "\n" \
                << "Backtrace: \n" \
                << qc::BacktraceToString(100, 2, "    "); \
                assert(x); \
            }

#endif