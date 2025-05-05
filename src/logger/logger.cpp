#include "logger.hpp"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

int var;

Logger::Logger() {
    m_logFile.open("log.txt", std::ios::app);
    if (!m_logFile.is_open()) {
        std::cerr << "Failed to open log file." << std::endl;
    }
}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::stringstream ss;

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");

    if (m_logFile.is_open()) {
        m_logFile << "[" << ss.str() << "] "
                    << message << std::endl;
    } else {
        std::cerr << "Log file not open. Message: " << message << std::endl;
    }
}
