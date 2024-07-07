// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Main.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#pragma once

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
#include "GuiLogSink.h"
#include "Logger.h"
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

std::shared_ptr<GuiLogSink> guiSink;

Image loadImage(const char *filename);
