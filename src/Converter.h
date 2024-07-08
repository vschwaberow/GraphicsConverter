// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Converter.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#pragma once

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <iostream>

struct HiresResult
{
    std::vector<uint8_t> bitmap;
    std::vector<uint8_t> colorRAM;
};

struct MulticolorResult
{
    std::vector<uint8_t> bitmap;
    std::vector<uint8_t> screenRAM;
    std::vector<uint8_t> colorRAM;
    std::vector<uint8_t> backgroundColors;
};

class Converter
{
public:
    Converter() = default;
    ~Converter() = default;

    static void convertKoalaToPNG(const char *inputFile, const char *outputFile);
    static void convertPNGToKoala(const char *inputFile, const char *outputFile);

    std::vector<uint8_t> convertToBitmap(const std::vector<uint32_t> &inputImage, int width, int height);
    std::vector<uint8_t> convertToHires(const std::vector<uint32_t> &inputImage, int width, int height);
    std::vector<uint8_t> convertToMulticolor(const std::vector<uint32_t> &inputImage, int width, int height);

private:
};