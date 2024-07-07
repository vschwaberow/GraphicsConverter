// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Logger.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <spdlog/spdlog.h>
#include "GuiLogSink.h"

class Logger
{
public:
    static void initialize();
    static std::shared_ptr<spdlog::logger> getLogger();
    static std::shared_ptr<GuiLogSink> getGuiSink();

private:
    static std::shared_ptr<spdlog::logger> logger;
    static std::shared_ptr<GuiLogSink> guiSink;
};

#endif // LOGGER_H