#include "qc/log/warp.h"
#include <cstdarg>
#include "qc/log/event.h"
#include "qc/log/logger.h"
#include "qc/log/manager.h"
#include "qc/macro.h"

qc::log::Warp::Warp(Logger::ptr logger, Event::ptr event) 
    : m_event(event), m_logger(logger) {}

qc::log::Warp::~Warp() {
    if (!m_event) {
        return;
    }
    if (m_logger->log(m_event)) {
        return;
    }
    // m_root->error("The logger is invalid, event will be logged by the root logger.");
    LOG_ERROR(m_root) << "The logger is invalid, event will be logged by the root logger.";
    m_event->appendName("(root)");
    m_root->log(m_event);
}

void qc::log::Warp::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    m_event->format(fmt, al);
    va_end(al);
}

qc::log::Logger::ptr qc::log::Warp::m_root = qc::log::Manager::GetInstance()->getRoot();