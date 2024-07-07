// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Main.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef MAIN_H
#define MAIN_H

#include <cstddef>
#include <vector>
#include <iostream>
#include <nfd.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "Converter.h"
#include "ColorReducer.h"
#include "Dithering.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

struct Image
{
    int width;
    int height;
    int channels;
    unsigned char *data;
    GLuint textureID;
};

struct LogMessage
{
    spdlog::level::level_enum level;
    std::string message;
};

GLuint originalTextureID = 0;
GLuint convertedTextureID = 0;
int originalImageWidth = 0;
int originalImageHeight = 0;
int convertedImageWidth = 0;
int convertedImageHeight = 0;

Image originalImage;
Image processedImage;
bool imageLoaded = false;
bool showDebugWindow = false;

std::deque<LogMessage> logBuffer;
const size_t MAX_LOG_BUFFER_SIZE = 1000;

Image loadImage(const char *filename);

#endif // MAIN_H