#include "qc/log/warp.h"

using namespace qc::log;

Warp::Warp(Logger::ptr logger, Event::ptr event) 
    : m_event(event), m_logger(logger) {}

Warp::~Warp() {
    if (!m_event) {
        return;
    }
    if (m_logger->log(m_event)) {
        return;
    }
    m_root->error("The logger is invalid, event will be logged by the root logger.");
    m_event->appendName("(root)");
    m_root->log(m_event);
}

Logger::ptr Warp::m_root = LoggerMgr::Instance()->getRoot();