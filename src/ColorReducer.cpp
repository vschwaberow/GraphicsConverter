// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/ColorReducer.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "ColorReducer.h"

std::vector<uint32_t> ColorReducer::reduceColors(const std::vector<uint32_t> &image, int width, int height, int targetColors, ColorReductionAlgorithm algo)
{
    switch (algo)
    {
    case ColorReductionAlgorithm::MedianCut:
        return medianCut(image, targetColors);
    case ColorReductionAlgorithm::KMeans:
        return kMeans(image, targetColors);
    case ColorReductionAlgorithm::OctreeQuantization:
        return octreeQuantization(image, targetColors);
    default:
        return image;
    }
}

std::vector<uint32_t> ColorReducer::medianCut(const std::vector<uint32_t> &image, int targetColors)
{
    // TODO: Implement median cut algorithm
    return image;
}

std::vector<uint32_t> ColorReducer::kMeans(const std::vector<uint32_t> &image, int targetColors)
{
    // TODO: Implement k-means algorithm
    return image;
}

std::vector<uint32_t> ColorReducer::octreeQuantization(const std::vector<uint32_t> &image, int targetColors)
{
    // TODO: Implement octree quantization algorithm
    return image;
}