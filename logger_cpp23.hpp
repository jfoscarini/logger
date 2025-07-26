/*
 * logger.hpp
 * Copyright (c) 2025 João Pedro Foscarini
 * SPDX-License-Identifier: MIT
 *
 * This file is licensed under the MIT License.
 * You may obtain a copy of the license at:
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>
#include <print>
#include <source_location>
#include <sstream>

namespace log {

enum class Level { Trace, Debug, Info, Notice, Warning, Error, Critical, Alert, Emergency, Profile };

class Logger {
  public:
    explicit Logger(Level level, const std::string& category = {})
        : m_stream() {
        m_stream << "[" << timestamp() << "][" << colorCode(level)
                 << levelLabel(level) << "\033[0m]";
        if (!category.empty()) m_stream << "[" << category << "]";
        m_stream << " ";
    }

    ~Logger() {
        m_stream << "\033[0m";
        std::clog << m_stream.str() << '\n';
    }

    template <typename T> Logger& operator<<(const T& value) {
        m_stream << value;
        return *this;
    }

  private:
    std::ostringstream m_stream;

    std::string timestamp() const {
        using namespace std::chrono;
        auto    now = system_clock::now();
        auto    ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
        auto    itt = system_clock::to_time_t(now);
        std::tm tm;
#if defined(_WIN32)
        localtime_s(&tm, &itt);
#else
        localtime_r(&itt, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%T") << "." << std::setw(3)
            << std::setfill('0') << ms.count();
        return oss.str();
    }

    static const char* colorCode(Level level) {
        switch (level) {
            case Level::Trace:     return "\033[1;37m";
            case Level::Debug:     return "\033[1;34m";
            case Level::Info:      return "\033[1;32m";
            case Level::Notice:    return "\033[1;36m";
            case Level::Warning:   return "\033[1;33m";
            case Level::Error:     return "\033[1;31m";
            case Level::Critical:  return "\033[1;35m";
            case Level::Alert:     return "\033[1;41m";
            case Level::Emergency: return "\033[1;41;97m";
            case Level::Profile:   return "\033[1;36m";
            default:               return "\033[0m";
        }
    }

    static const char* levelLabel(Level level) {
        switch (level) {
            case Level::Trace:     return "  TRACE  ";
            case Level::Debug:     return "  DEBUG  ";
            case Level::Info:      return "  INFO   ";
            case Level::Notice:    return " NOTICE  ";
            case Level::Warning:   return " WARNING ";
            case Level::Error:     return "  ERROR  ";
            case Level::Critical:  return "CRITICAL ";
            case Level::Alert:     return "  ALERT  ";
            case Level::Emergency: return "EMERGENCY";
            case Level::Profile:   return "PROFILING";
            default:               return " UNKNOWN ";
        }
    }
};

#ifdef NDEBUG
#define log_trace(...)     if (true) {} else log::Logger(log::Level::Trace, #__VA_ARGS__)
#define log_debug(...)     if (true) {} else log::Logger(log::Level::Debug, #__VA_ARGS__)
#define log_info(...)      if (true) {} else log::Logger(log::Level::Info, #__VA_ARGS__)
#define log_notice(...)    if (true) {} else log::Logger(log::Level::Notice, #__VA_ARGS__)
#define log_warning(...)   if (true) {} else log::Logger(log::Level::Warning, #__VA_ARGS__)
#define log_error(...)     if (true) {} else log::Logger(log::Level::Error, #__VA_ARGS__)
#define log_critical(...)  if (true) {} else log::Logger(log::Level::Critical, #__VA_ARGS__)
#define log_alert(...)     if (true) {} else log::Logger(log::Level::Alert, #__VA_ARGS__)
#define log_emergency(...) if (true) {} else log::Logger(log::Level::Emergency, #__VA_ARGS__)
#define log_profiling(...) if (true) {} else log::Logger(log::Level::Profile, #__VA_ARGS__)
#else
#define log_trace(...)     log::Logger(log::Level::Trace, #__VA_ARGS__)
#define log_debug(...)     log::Logger(log::Level::Debug, #__VA_ARGS__)
#define log_info(...)      log::Logger(log::Level::Info, #__VA_ARGS__)
#define log_notice(...)    log::Logger(log::Level::Notice, #__VA_ARGS__)
#define log_warning(...)   log::Logger(log::Level::Warning, #__VA_ARGS__)
#define log_error(...)     log::Logger(log::Level::Error, #__VA_ARGS__)
#define log_critical(...)  log::Logger(log::Level::Critical, #__VA_ARGS__)
#define log_alert(...)     log::Logger(log::Level::Alert, #__VA_ARGS__)
#define log_emergency(...) log::Logger(log::Level::Emergency, #__VA_ARGS__)
#define log_profiling(...) log::Logger(log::Level::Profile, #__VA_ARGS__)
#endif

class ScopeLogger {
  public:
    explicit ScopeLogger(
        std::string_view            tag = {},
        const std::source_location& location = std::source_location::current())
        : m_tag(tag),
          m_file_name(location.file_name()),
          m_line(location.line()),
          m_start(std::chrono::steady_clock::now()) {
        log_profiling() << std::format(
            "START {} @ {}:{}", m_tag, m_file_name, m_line);
    }

    ~ScopeLogger() noexcept {
        auto now = std::chrono::steady_clock::now();
        auto elapsed_us =
            std::chrono::duration_cast<std::chrono::microseconds>(now - m_start)
                .count();
        double                 duration_ms = elapsed_us / 1000.0;
        const std::string_view leave_message =
            (std::uncaught_exceptions() == 0) ? "FINISH" : "EXCEPTION!";
        log_profiling() << std::format("{} {} ({:0.3f}ms) @ {}:{}",
                                       leave_message,
                                       m_tag,
                                       duration_ms,
                                       m_file_name,
                                       m_line);
    }

    ScopeLogger(const ScopeLogger&) = delete;
    ScopeLogger& operator=(const ScopeLogger&) = delete;

    ScopeLogger(ScopeLogger&&) = default;
    ScopeLogger& operator=(ScopeLogger&&) = default;

  private:
    std::string_view                      m_tag;
    std::string_view                      m_file_name;
    uint32_t                              m_line;
    std::chrono::steady_clock::time_point m_start;
};

#ifdef NDEBUG
#define log_profile(tag)
#else
#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define log_profile(...) log::ScopeLogger CONCAT(_scopelogger_, __LINE__) { #__VA_ARGS__ }
#endif

} // namespace log
