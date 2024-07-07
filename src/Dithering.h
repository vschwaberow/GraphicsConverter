// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Dithering.h
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#pragma once

#include <vector>
#include <cstdint>
#include <span>
#include <array>
#include <algorithm>
#include <span>
#include <string>

enum class DitheringAlgorithm
{
    FloydSteinberg,
    Bayer,
    Ordered
};

class Dithering
{
public:
    static std::vector<uint32_t> applyDithering(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette, DitheringAlgorithm algo);
    static std::string getAlgorithmName(DitheringAlgorithm algo);

private:
    static std::vector<uint32_t> floydSteinberg(std::span<const uint32_t> image, int width, int height, std::span<const uint32_t> palette);
    static std::vector<uint32_t> bayer(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette);
    static std::vector<uint32_t> ordered(const std::vector<uint32_t> &image, int width, int height, const std::vector<uint32_t> &palette);
    static uint32_t findClosestColor(int r, int g, int b, std::span<const uint32_t> palette);
    static void distributeError(std::vector<std::array<float, 3>> &error, const std::array<float, 3> &err,
                                int index, int x, int y, int width, int height);
    static constexpr std::array<int, 3> getRGB(uint32_t pixel);
};
