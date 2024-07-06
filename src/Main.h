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

struct Image
{
    int width;
    int height;
    int channels;
    unsigned char *data;
    GLuint textureID;
};

Image originalImage;
Image processedImage;
bool imageLoaded = false;

Image loadImage(const char *filename);

#endif // MAIN_H