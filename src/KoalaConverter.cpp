// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/KoalaConverter.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "KoalaConverter.h"
#include <fstream>
#include <stdexcept>


void KoalaConverter::convertImage(const std::vector<uint8_t> &pixelData, int width, int height)
{
    if (width != KOALA_WIDTH || height != KOALA_HEIGHT)
    {
        throw std::runtime_error("Image dimensions must be 160x200 for Koala format");
    }

    m_bitmap.resize(KOALA_BITMAP_SIZE);
    m_screenRam.resize(KOALA_SCREEN_RAM_SIZE);
    m_colorRam.resize(KOALA_COLOR_RAM_SIZE);

    // TODO: Implement proper color quantization, simplification, and dithering
    for (int y = 0; y < KOALA_HEIGHT; y++)
    {
        for (int x = 0; x < KOALA_WIDTH; x++)
        {
            int pixelIndex = (y * KOALA_WIDTH + x) * 3;
            uint8_t r = pixelData[pixelIndex];
            uint8_t g = pixelData[pixelIndex + 1];
            uint8_t b = pixelData[pixelIndex + 2];

            uint8_t colorIndex = (r > 128 ? 4 : 0) | (g > 128 ? 2 : 0) | (b > 128 ? 1 : 0);

            int charX = x / 4;
            int charY = y / 8;
            int charIndex = charY * 40 + charX;
            int bitIndex = (y % 8) * 8 + (x % 4) * 2;

            m_bitmap[charIndex * 8 + bitIndex / 8] |= (colorIndex & 3) << (6 - (bitIndex % 8));

            if (x % 4 == 0 && y % 8 == 0)
            {
                m_screenRam[charIndex] = colorIndex & 0xF;
                m_colorRam[charIndex] = (colorIndex >> 4) & 0xF;
            }
        }
    }

    m_backgroundColor = 0;
}

void KoalaConverter::saveFile(const std::string &filename) const
{
    std::ofstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Unable to open file for writing");
    }

    uint16_t loadAddress = 0x6000;
    file.write(reinterpret_cast<const char *>(&loadAddress), KOALA_HEADER_SIZE);
    file.write(reinterpret_cast<const char *>(m_bitmap.data()), KOALA_BITMAP_SIZE);
    file.write(reinterpret_cast<const char *>(m_screenRam.data()), KOALA_SCREEN_RAM_SIZE);
    file.write(reinterpret_cast<const char *>(m_colorRam.data()), KOALA_COLOR_RAM_SIZE);
    file.write(reinterpret_cast<const char *>(&m_backgroundColor), KOALA_BACKGROUND_COLOR_SIZE);

    if (!file)
    {
        // TODO: Handle error with spdlog
        throw std::runtime_error("Error writing to file");
    }
}