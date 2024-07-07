// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/GuiLogSink.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "GuiLogSink.h"

GuiLogSink::GuiLogSink() {}

const std::deque<LogMessage> &GuiLogSink::getLogBuffer() const
{
    return logBuffer;
}

void GuiLogSink::sink_it_(const spdlog::details::log_msg &msg)
{
    spdlog::memory_buf_t formatted;
    spdlog::sinks::base_sink<std::mutex>::formatter_->format(msg, formatted);
    logBuffer.push_back({msg.level, fmt::to_string(formatted)});
    if (logBuffer.size() > MAX_LOG_BUFFER_SIZE)
    {
        logBuffer.pop_front();
    }
}

void GuiLogSink::flush_() {}