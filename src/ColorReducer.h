// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/ColorReducer.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#pragma once

#include <vector>
#include <cstdint>
#include <algorithm>
#include <queue>
#include <cmath>
#include <limits>
#include <random>

enum class ColorReductionAlgorithm
{
    MedianCut,
    KMeans,
    OctreeQuantization
};

struct ColorBox
{
    std::vector<uint32_t> pixels;
    int minR, maxR, minG, maxG, minB, maxB;

    ColorBox(const std::vector<uint32_t> &pixels) : pixels(pixels)
    {
        minR = minG = minB = 255;
        maxR = maxG = maxB = 0;
        for (uint32_t pixel : pixels)
        {
            int r = (pixel >> 16) & 0xFF;
            int g = (pixel >> 8) & 0xFF;
            int b = pixel & 0xFF;
            minR = std::min(minR, r);
            maxR = std::max(maxR, r);
            minG = std::min(minG, g);
            maxG = std::max(maxG, g);
            minB = std::min(minB, b);
            maxB = std::max(maxB, b);
        }
    }

    int getLongestSideIndex() const
    {
        int rLength = maxR - minR;
        int gLength = maxG - minG;
        int bLength = maxB - minB;
        if (rLength >= gLength && rLength >= bLength)
            return 0;
        if (gLength >= rLength && gLength >= bLength)
            return 1;
        return 2;
    }
};

struct Color
{
    int r, g, b;
    Color() : r(0), g(0), b(0) {}
    Color(uint32_t pixel) : r((pixel >> 16) & 0xFF), g((pixel >> 8) & 0xFF), b(pixel & 0xFF) {}
    uint32_t toUint32() const { return (r << 16) | (g << 8) | b; }
};

class OctreeNode
{
public:
    OctreeNode() : red(0), green(0), blue(0), pixelCount(0), isLeaf(false)
    {
        for (int i = 0; i < 8; ++i)
        {
            children[i] = nullptr;
        }
    }

    ~OctreeNode()
    {
        for (int i = 0; i < 8; ++i)
        {
            delete children[i];
        }
    }

    void addColor(uint32_t color, int level, int maxLevel, int &leafCount, int maxLeaves)
    {
        ++pixelCount;
        red += (color >> 16) & 0xFF;
        green += (color >> 8) & 0xFF;
        blue += color & 0xFF;

        if (level >= maxLevel)
        {
            isLeaf = true;
            return;
        }

        int index = getColorIndex(color, level);
        if (children[index] == nullptr)
        {
            if (leafCount >= maxLeaves)
                return;
            children[index] = new OctreeNode();
            ++leafCount;
        }
        children[index]->addColor(color, level + 1, maxLevel, leafCount, maxLeaves);
    }

    uint32_t getColor() const
    {
        if (pixelCount == 0)
            return 0;
        int r = red / pixelCount;
        int g = green / pixelCount;
        int b = blue / pixelCount;
        return (r << 16) | (g << 8) | b;
    }

    OctreeNode *children[8];
    int red, green, blue;
    int pixelCount;
    bool isLeaf;

private:
    int getColorIndex(uint32_t color, int level)
    {
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;
        int index = 0;
        int mask = 0x80 >> level;
        if (r & mask)
            index |= 4;
        if (g & mask)
            index |= 2;
        if (b & mask)
            index |= 1;
        return index;
    }
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
