// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Logger.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "Logger.h"
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> Logger::logger;
std::shared_ptr<GuiLogSink> Logger::guiSink;

void Logger::initialize()
{
    try
    {
        guiSink = std::make_shared<GuiLogSink>();
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/gfxconverter.log");

        logger = std::make_shared<spdlog::logger>("gfxconverter", spdlog::sinks_init_list{guiSink, fileSink});
        spdlog::set_default_logger(logger);
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_on(spdlog::level::debug);
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cerr << "Log initialization failed: " << ex.what() << std::endl;
    }
}

std::shared_ptr<spdlog::logger> Logger::getLogger()
{
    return logger;
}

std::shared_ptr<GuiLogSink> Logger::getGuiSink()
{
    return guiSink;
}