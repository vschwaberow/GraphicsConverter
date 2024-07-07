// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Dithering.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "Dithering.h"


std::string Dithering::getAlgorithmName(DitheringAlgorithm algo)
{
    switch (algo)
    {
    case DitheringAlgorithm::FloydSteinberg:
        return "Floyd-Steinberg";
    case DitheringAlgorithm::Bayer:
        return "Bayer";
    case DitheringAlgorithm::Ordered:
        return "Ordered";
    default:
        return "Unknown";
    }
}

std::vector<uint32_t> Dithering::applyDithering(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette, DitheringAlgorithm algo)
{
    switch (algo)
    {
    case DitheringAlgorithm::FloydSteinberg:
        return floydSteinberg(image, width, height, palette);
    case DitheringAlgorithm::Bayer:
        return bayer(image, width, height, palette);
    case DitheringAlgorithm::Ordered:
        return ordered(image, width, height, palette);
    default:
        return image;
    }
}

constexpr std::array<int, 3> Dithering::getRGB(uint32_t pixel)
{
    return {
        static_cast<int>((pixel >> 16) & 0xFF),
        static_cast<int>((pixel >> 8) & 0xFF),
        static_cast<int>(pixel & 0xFF)};
}

uint32_t Dithering::findClosestColor(int r, int g, int b, std::span<const uint32_t> palette)
{
    return *std::min_element(palette.begin(), palette.end(),
                             [r, g, b](uint32_t c1, uint32_t c2)
                             {
                                 int r1 = (c1 >> 16) & 0xFF;
                                 int g1 = (c1 >> 8) & 0xFF;
                                 int b1 = c1 & 0xFF;
                                 int r2 = (c2 >> 16) & 0xFF;
                                 int g2 = (c2 >> 8) & 0xFF;
                                 int b2 = c2 & 0xFF;

                                 int dist1 = (r - r1) * (r - r1) + (g - g1) * (g - g1) + (b - b1) * (b - b1);
                                 int dist2 = (r - r2) * (r - r2) + (g - g2) * (g - g2) + (b - b2) * (b - b2);

                                 return dist1 < dist2;
                             });
}

void Dithering::distributeError(std::vector<std::array<float, 3>> &error, const std::array<float, 3> &err,
                                int index, int x, int y, int width, int height)
{
    constexpr std::array<std::pair<int, int>, 4> offsets = {{{1, 0}, {-1, 1}, {0, 1}, {1, 1}}};
    constexpr std::array<float, 4> factors = {7.0f / 16, 3.0f / 16, 5.0f / 16, 1.0f / 16};

    for (size_t i = 0; i < offsets.size(); ++i)
    {
        int nx = x + offsets[i].first;
        int ny = y + offsets[i].second;
        if (nx >= 0 && nx < width && ny < height)
        {
            int nindex = ny * width + nx;
            for (int j = 0; j < 3; ++j)
            {
                error[nindex][j] += err[j] * factors[i];
            }
        }
    }
}

std::vector<uint32_t> Dithering::floydSteinberg(std::span<const uint32_t> image, int width, int height, std::span<const uint32_t> palette)
{

    std::vector<uint32_t> result(image.begin(), image.end());
    std::vector<std::array<float, 3>> error(width * height, {0.0f, 0.0f, 0.0f});

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;
            auto [oldR, oldG, oldB] = getRGB(result[index]);

            oldR = std::clamp(oldR + static_cast<int>(error[index][0]), 0, 255);
            oldG = std::clamp(oldG + static_cast<int>(error[index][1]), 0, 255);
            oldB = std::clamp(oldB + static_cast<int>(error[index][2]), 0, 255);

            uint32_t newPixel = findClosestColor(oldR, oldG, oldB, palette);
            result[index] = newPixel;

            auto [newR, newG, newB] = getRGB(newPixel);
            std::array<float, 3> err = {
                static_cast<float>(oldR - newR),
                static_cast<float>(oldG - newG),
                static_cast<float>(oldB - newB)};

            distributeError(error, err, index, x, y, width, height);
        }
    }

    return result;
}

std::vector<uint32_t> Dithering::bayer(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette)
{
    std::vector<uint32_t> result(image.size());

    const int bayerMatrix[4][4] = {
        {0, 8, 2, 10},
        {12, 4, 14, 6},
        {3, 11, 1, 9},
        {15, 7, 13, 5}};

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;
            uint32_t pixel = image[index];

            std::array<int, 3> rgb = getRGB(pixel);

            // Apply Bayer dithering
            int threshold = bayerMatrix[y % 4][x % 4];
            for (int i = 0; i < 3; ++i)
            {
                rgb[i] = std::clamp(rgb[i] + (threshold - 8) * 4, 0, 255);
            }

            uint32_t newPixel = findClosestColor(rgb[0], rgb[1], rgb[2], palette);
            result[index] = newPixel;
        }
    }

    return result;
}

std::vector<uint32_t> Dithering::ordered(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette)
{
    std::vector<uint32_t> result(image.size());

    const int orderedMatrix[8][8] = {
        {0, 48, 12, 60, 3, 51, 15, 63},
        {32, 16, 44, 28, 35, 19, 47, 31},
        {8, 56, 4, 52, 11, 59, 7, 55},
        {40, 24, 36, 20, 43, 27, 39, 23},
        {2, 50, 14, 62, 1, 49, 13, 61},
        {34, 18, 46, 30, 33, 17, 45, 29},
        {10, 58, 6, 54, 9, 57, 5, 53},
        {42, 26, 38, 22, 41, 25, 37, 21}};

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int index = y * width + x;
            uint32_t pixel = image[index];

            std::array<int, 3> rgb = getRGB(pixel);

            int threshold = orderedMatrix[y % 8][x % 8];
            for (int i = 0; i < 3; ++i)
            {
                rgb[i] = std::clamp(rgb[i] + (threshold - 32) * 2, 0, 255);
            }

            uint32_t newPixel = findClosestColor(rgb[0], rgb[1], rgb[2], palette);
            result[index] = newPixel;
        }
    }

    return result;
}