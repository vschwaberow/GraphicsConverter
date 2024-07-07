// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/ImageConverter.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#pragma once

#include <vector>
#include <string>
#include <cstdint>

class ImageConverter
{
public:
    virtual ~ImageConverter() = default;
    virtual void convertImage(const std::vector<uint8_t> &pixelData, int width, int height) = 0;
    virtual void saveFile(const std::string &filename) const = 0;
};
