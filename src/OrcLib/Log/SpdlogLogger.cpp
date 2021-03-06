//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// Copyright © 2020 ANSSI. All Rights Reserved.
//
// Author(s): fabienfl
//

#pragma once

#include "Log/SpdlogLogger.h"

namespace Orc {
namespace Log {

SpdlogLogger::SpdlogLogger(const std::string& name)
    : m_logger(std::make_shared<spdlog::logger>(name))
    , m_backtraceFormatter(
          std::make_unique<spdlog::pattern_formatter>(kDefaultLogPattern, spdlog::pattern_time_type::utc))
{
}

void SpdlogLogger::Add(SpdlogSink::Ptr sink)
{
    sink->AddTo(*m_logger);
    m_sinks.push_back(std::move(sink));
}

spdlog::level::level_enum SpdlogLogger::Level() const
{
    return m_logger->level();
}

void SpdlogLogger::SetLevel(spdlog::level::level_enum level)
{
    m_logger->set_level(level);
}

void SpdlogLogger::EnableBacktrace(size_t messageCount)
{
    m_logger->enable_backtrace(messageCount);
}

void SpdlogLogger::DisableBacktrace()
{
    m_logger->disable_backtrace();
}

void SpdlogLogger::DumpBacktrace()
{
    // Backup log settings
    const auto loggerLevel = m_logger->level();
    m_logger->set_level(spdlog::level::trace);

    struct SinkSettings
    {
        spdlog::level::level_enum level;
        std::unique_ptr<spdlog::formatter> formatter;
    };

    std::vector<SinkSettings> sinksSettings(m_sinks.size());
    for (size_t i = 0; i < m_sinks.size(); ++i)
    {
        sinksSettings[i] = {m_sinks[i]->Level(), m_sinks[i]->CloneFormatter()};

        m_sinks[i]->SetLevel(spdlog::level::trace);
        m_sinks[i]->SetFormatter(m_backtraceFormatter->clone());
    }

    m_logger->dump_backtrace();

    // Restore log settings
    for (size_t i = 0; i < m_sinks.size(); ++i)
    {
        m_sinks[i]->SetLevel(sinksSettings[i].level);
        m_sinks[i]->SetFormatter(std::move(sinksSettings[i].formatter));
    }

    SetLevel(loggerLevel);
}

void SpdlogLogger::SetErrorHandler(std::function<void(const std::string&)> handler)
{
    m_logger->set_error_handler(handler);
}

void SpdlogLogger::SetFormatter(std::unique_ptr<spdlog::formatter> formatter)
{
    if (formatter == nullptr)
    {
        // spdlog sinks does not allow set_formatter with nullptr
        return;
    }

    // see spldog implementation, spdlog::logger does not keep any formatter reference
    for (auto& sink : m_sinks)
    {
        sink->SetFormatter(formatter->clone());
    }
}

void SpdlogLogger::SetPattern(const std::string& pattern)
{
    auto formatter = std::make_unique<spdlog::pattern_formatter>(pattern);
    SetFormatter(std::move(formatter));
}

void SpdlogLogger::SetAsDefaultLogger()
{
    spdlog::set_default_logger(m_logger);
}

const std::vector<SpdlogSink::Ptr>& SpdlogLogger::Sinks()
{
    return m_sinks;
}

}  // namespace Log
}  // namespace Orc
