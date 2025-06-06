#include "qc/log/log_module.h"
#include "qc/utils.h"

using namespace qc::log;

int main() {
    int x = 10;
    char c = 'A';
    // 创建日志器
    Logger::ptr logger = LoggerMgr::Instance()->getLogger("test_logger");
    logger->setLayout(LoggerMgr::Instance()->getLayout());
    logger->setLevel(LoggerMgr::Instance()->getLevel());
    logger->setPattern(LoggerMgr::Instance()->getPattern());
    logger->setValid(true);

    // 创建控制台输出器
    ConsoleAppender::ptr consoleAppender = std::make_unique<ConsoleAppender>(Level::INFO);
    logger->addAppender(std::move(consoleAppender));

    // 创建文件输出器
    FileAppender::ptr fileAppender = std::make_unique<FileAppender>("test.log", Level::WARN);
    fileAppender->setLayout(std::make_shared<Layout>("%d{%Y-%m-%d %H:%M:%S}%T%t%T[%p]%T[%c]%T%f:%l%T%m%n"));
    logger->addAppender(std::move(fileAppender));

    // 记录日志
    // logger->debug("This is a debug message. %d %c", x, c);
    // logger->info("This is an info message. %d %c", x, c);
    // logger->warn("This is a warning message. %d %c", x, c);
    // logger->error("This is an error message. %d %c", x, c);
    // logger->fatal("This is a fatal message. %d %c", x, c);

    // 使用宏记录日志
    LOG_DEBUG(logger) << "This is a define debug log message." << x << c;
    LOG_INFO(logger) << "This is an define info log message." << x << c;
    LOG_WARN(logger) << "This is a define warning log message." << x << c;
    LOG_ERROR(logger) << "This is an define error log message." << x << c;
    LOG_FATAL(logger) << "This is a define fatal log message." << x << c;

    // 使用格式化宏记录日志
    LOG_FMT_DEBUG(logger, "This is a formatted debug log message with value: ");
    LOG_FMT_INFO(logger, "This is a formatted info log message with value: ");
    LOG_FMT_WARN(logger, "This is a formatted warning log message with value: %d %c", x, c);
    LOG_FMT_ERROR(logger, "This is a formatted error log message with value: %d %c", x, c);
    LOG_FMT_FATAL(logger, "This is a formatted fatal log message with value: %d %c", x, c);

    auto l = LoggerMgr::Instance()->getLogger("XX");
    LOG_DEBUG(l) << "This is a debug log message from logger XX.";
    LOG_DEBUG(ROOT_LOG()) << "This is a debug log message from root logger.";

    return 0;
}