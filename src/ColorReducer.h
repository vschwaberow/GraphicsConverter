// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/ColorReducer.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#ifndef COLOR_REDUCER_H
#define COLOR_REDUCER_H
#include <vector>
#include <cstdint>

enum class ColorReductionAlgorithm
{
    MedianCut,
    KMeans,
    OctreeQuantization
};

class ColorReducer
{
public:
    static std::vector<uint32_t> reduceColors(const std::vector<uint32_t> &image, int width, int height, int targetColors, ColorReductionAlgorithm algo);

private:
    static std::vector<uint32_t> medianCut(const std::vector<uint32_t> &image, int targetColors);
    static std::vector<uint32_t> kMeans(const std::vector<uint32_t> &image, int targetColors);
    static std::vector<uint32_t> octreeQuantization(const std::vector<uint32_t> &image, int targetColors);
};

#endif // COLOR_REDUCER_H