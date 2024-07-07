// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/GuiLogSink.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef GUI_LOG_SINK_H
#define GUI_LOG_SINK_H

#include <spdlog/sinks/base_sink.h>
#include <deque>
#include <mutex>
#include <new>

struct LogMessage
{
    spdlog::level::level_enum level;
    std::string message;
};

class GuiLogSink : public spdlog::sinks::base_sink<std::mutex>
{
public:
    GuiLogSink();
    const std::deque<LogMessage> &getLogBuffer() const;

protected:
    void sink_it_(const spdlog::details::log_msg &msg) override;
    void flush_() override;

private:
    std::deque<LogMessage> logBuffer;
    const size_t MAX_LOG_BUFFER_SIZE = 1000;
};

#endif // GUI_LOG_SINK_H