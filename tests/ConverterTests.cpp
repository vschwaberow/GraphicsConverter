// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: tests/ConverterTests.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include <gtest/gtest.h>
#include "Converter.h"
#include <vector>
#include <cstdint>

class ConverterTest : public ::testing::Test
{
protected:
    Converter converter;
};

TEST_F(ConverterTest, ConvertToBitmap)
{
    std::vector<uint32_t> inputImage(320 * 200, 0xFF000000);
    int width = 320;
    int height = 200;

    auto result = converter.convertToBitmap(inputImage, width, height);

    EXPECT_EQ(result.size(), width * height / 8);
}

TEST_F(ConverterTest, ConvertToHires)
{
    std::vector<uint32_t> inputImage(320 * 200, 0xFF000000);
    int width = 320;
    int height = 200;

    auto result = converter.convertToHires(inputImage, width, height);

    HiresResult hiresResult = *reinterpret_cast<HiresResult *>(&result);

    EXPECT_EQ(hiresResult.bitmap.size(), width * height / 8);
    EXPECT_EQ(hiresResult.colorRAM.size(), width * height / 8);
}

TEST_F(ConverterTest, ConvertToMulticolor)
{
    std::vector<uint32_t> inputImage(320 * 200, 0xFF000000);
    int width = 320;
    int height = 200;

    auto result = converter.convertToMulticolor(inputImage, width, height);

    MulticolorResult multicolorResult = *reinterpret_cast<MulticolorResult *>(&result);

    EXPECT_EQ(multicolorResult.bitmap.size(), width * height / 8);
    EXPECT_EQ(multicolorResult.screenRAM.size(), width * height / 40);
    EXPECT_EQ(multicolorResult.colorRAM.size(), width * height / 40);
    EXPECT_EQ(multicolorResult.backgroundColors.size(), 4);
}

TEST_F(ConverterTest, HandleEmptyInput)
{
    std::vector<uint32_t> emptyImage;
    int width = 0;
    int height = 0;

    EXPECT_THROW(converter.convertToBitmap(emptyImage, width, height), std::invalid_argument);
    EXPECT_THROW(converter.convertToHires(emptyImage, width, height), std::invalid_argument);
    EXPECT_THROW(converter.convertToMulticolor(emptyImage, width, height), std::invalid_argument);
}

TEST_F(ConverterTest, HandleInvalidDimensions)
{
    std::vector<uint32_t> inputImage(100, 0xFF000000);
    int width = -1;
    int height = -1;

    EXPECT_THROW(converter.convertToBitmap(inputImage, width, height), std::invalid_argument);
    EXPECT_THROW(converter.convertToHires(inputImage, width, height), std::invalid_argument);
    EXPECT_THROW(converter.convertToMulticolor(inputImage, width, height), std::invalid_argument);
}