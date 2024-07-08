// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: tests/ColorReducerTests.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include <gtest/gtest.h>
#include "ColorReducer.h"
#include <vector>
#include <set>
#include <algorithm>

class ColorReducerTest : public ::testing::Test
{
protected:
    std::vector<uint32_t> testImage;

    void SetUp() override
    {
        testImage = {
            0xFF0000FF, 0x00FF00FF, 0x0000FFFF, 0xFFFFFFFF,
            0x000000FF, 0xFF00FFFF, 0xFFFF00FF, 0x00FFFFFF,
            0x808080FF, 0xC0C0C0FF, 0x404040FF, 0x800000FF,
            0x008000FF, 0x000080FF, 0x808000FF, 0x800080FF};
    }
};

TEST_F(ColorReducerTest, ReduceColorsMedianCut)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::MedianCut);

    ASSERT_EQ(result.size(), testImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_LE(uniqueColors.size(), 8);
}

TEST_F(ColorReducerTest, ReduceColorsKMeans)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::KMeans);

    ASSERT_EQ(result.size(), testImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_LE(uniqueColors.size(), 8);
}

TEST_F(ColorReducerTest, ReduceColorsOctree)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::OctreeQuantization);

    ASSERT_EQ(result.size(), testImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_LE(uniqueColors.size(), 8);
}

TEST_F(ColorReducerTest, PreservesAlpha)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::MedianCut);

    for (size_t i = 0; i < result.size(); ++i)
    {
        EXPECT_EQ(result[i] & 0xFF, testImage[i] & 0xFF) << "Alpha channel mismatch at index " << i;
    }
}

TEST_F(ColorReducerTest, HandlesSmallColorPalette)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 2, ColorReductionAlgorithm::MedianCut);

    ASSERT_EQ(result.size(), testImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_LE(uniqueColors.size(), 2);
}

TEST_F(ColorReducerTest, HandlesLargeColorPalette)
{
    std::vector<uint32_t> result = ColorReducer::reduceColors(testImage, 4, 4, 16, ColorReductionAlgorithm::MedianCut);

    ASSERT_EQ(result.size(), testImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_LE(uniqueColors.size(), 16);
}

TEST_F(ColorReducerTest, ConsistentResults)
{
    std::vector<uint32_t> result1 = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::MedianCut);
    std::vector<uint32_t> result2 = ColorReducer::reduceColors(testImage, 4, 4, 8, ColorReductionAlgorithm::MedianCut);

    EXPECT_EQ(result1, result2);
}

TEST_F(ColorReducerTest, HandlesMonochromeImage)
{
    std::vector<uint32_t> monochromeImage(16, 0xFF0000FF);
    std::vector<uint32_t> result = ColorReducer::reduceColors(monochromeImage, 4, 4, 8, ColorReductionAlgorithm::MedianCut);

    ASSERT_EQ(result.size(), monochromeImage.size());

    std::set<uint32_t> uniqueColors(result.begin(), result.end());
    EXPECT_EQ(uniqueColors.size(), 1);

    uint32_t resultColor = *uniqueColors.begin();
    uint8_t r = (resultColor >> 24) & 0xFF;
    uint8_t g = (resultColor >> 16) & 0xFF;
    uint8_t b = (resultColor >> 8) & 0xFF;
    uint8_t a = resultColor & 0xFF;

    EXPECT_EQ(r, 255) << "Red channel mismatch";
    EXPECT_EQ(g, 0) << "Green channel mismatch";
    EXPECT_EQ(b, 0) << "Blue channel mismatch";
    EXPECT_EQ(a, 255) << "Alpha channel mismatch";

    EXPECT_EQ(resultColor, 0xFF0000FF) << "Full color mismatch. Got: "
                                       << std::hex << resultColor
                                       << " (R:" << (int)r
                                       << " G:" << (int)g
                                       << " B:" << (int)b
                                       << " A:" << (int)a << ")";
}