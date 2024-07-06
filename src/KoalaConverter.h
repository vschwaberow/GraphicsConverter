// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/KoalaConverter.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef KOALACONVERTER_H
#define KOALACONVERTER_H

#include "ImageConverter.h"
#include <vector>
#include <cstdint>

class KoalaConverter : public ImageConverter
{
public:
    void convertImage(const std::vector<uint8_t> &pixelData, int width, int height) override;
    void saveFile(const std::string &filename) const override;

private:
    static constexpr int KOALA_WIDTH = 160;
    static constexpr int KOALA_HEIGHT = 200;
    static constexpr int KOALA_HEADER_SIZE = 2;
    static constexpr int KOALA_BITMAP_SIZE = 8000;
    static constexpr int KOALA_SCREEN_RAM_SIZE = 1000;
    static constexpr int KOALA_COLOR_RAM_SIZE = 1000;
    static constexpr int KOALA_BACKGROUND_COLOR_SIZE = 1;

    std::vector<uint8_t> m_bitmap;
    std::vector<uint8_t> m_screenRam;
    std::vector<uint8_t> m_colorRam;
    uint8_t m_backgroundColor;
};

#endif // KOALACONVERTER_H