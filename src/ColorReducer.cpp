// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/ColorReducer.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "ColorReducer.h"

int colorDistance(const Color &c1, const Color &c2)
{
    int dr = c1.r - c2.r;
    int dg = c1.g - c2.g;
    int db = c1.b - c2.b;
    return dr * dr + dg * dg + db * db;
}

std::string ColorReducer::getColorReducerName(ColorReductionAlgorithm algo)
{
    switch (algo)
    {
    case ColorReductionAlgorithm::MedianCut:
        return "MedianCut";
    case ColorReductionAlgorithm::KMeans:
        return "KMeans";
    case ColorReductionAlgorithm::OctreeQuantization:
        return "OctreeQuantization";
    default:
        return "Unknown";
    }
}

std::vector<uint32_t> ColorReducer::reduceColors(const std::vector<uint32_t> &image, int width, int height, int targetColors, ColorReductionAlgorithm algo)
{
    try
    {
        // Check if all pixels are the same, so its mono color
        if (std::adjacent_find(image.begin(), image.end(), std::not_equal_to<>()) == image.end())
        {
            return image;
        }
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
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return image;
    }
}

std::vector<uint32_t> ColorReducer::medianCut(const std::vector<uint32_t> &image, int targetColors)
{
    auto compareBoxes = [](const ColorBox &a, const ColorBox &b)
    {
        return a.pixels.size() < b.pixels.size();
    };
    std::priority_queue<ColorBox, std::vector<ColorBox>, decltype(compareBoxes)> boxes(compareBoxes);

    boxes.push(ColorBox(image));

    while (static_cast<int>(boxes.size()) < targetColors)
    {
        ColorBox box = boxes.top();
        boxes.pop();

        if (box.pixels.size() < 2)
        {
            boxes.push(box);
            break;
        }

        int splitIndex = box.getLongestSideIndex();
        std::sort(box.pixels.begin(), box.pixels.end(), [splitIndex](uint32_t a, uint32_t b)
                  { return ((a >> (8 * (2 - splitIndex))) & 0xFF) < ((b >> (8 * (2 - splitIndex))) & 0xFF); });

        size_t medianIndex = box.pixels.size() / 2;
        boxes.push(ColorBox(std::vector<uint32_t>(box.pixels.begin(), box.pixels.begin() + medianIndex)));
        boxes.push(ColorBox(std::vector<uint32_t>(box.pixels.begin() + medianIndex, box.pixels.end())));
    }

    std::vector<uint32_t> palette;
    while (!boxes.empty())
    {
        const ColorBox &box = boxes.top();
        int r = 0, g = 0, b = 0;
        for (uint32_t pixel : box.pixels)
        {
            r += (pixel >> 16) & 0xFF;
            g += (pixel >> 8) & 0xFF;
            b += pixel & 0xFF;
        }
        r /= box.pixels.size();
        g /= box.pixels.size();
        b /= box.pixels.size();
        palette.push_back((r << 16) | (g << 8) | b);
        boxes.pop();
    }

    std::vector<uint32_t> result(image.size());
    for (size_t i = 0; i < image.size(); ++i)
    {
        uint32_t pixel = image[i];
        int r = (pixel >> 16) & 0xFF;
        int g = (pixel >> 8) & 0xFF;
        int b = pixel & 0xFF;

        uint32_t closestColor = palette[0];
        int minDistance = std::numeric_limits<int>::max();

        for (uint32_t paletteColor : palette)
        {
            int pr = (paletteColor >> 16) & 0xFF;
            int pg = (paletteColor >> 8) & 0xFF;
            int pb = paletteColor & 0xFF;

            int distance = (r - pr) * (r - pr) + (g - pg) * (g - pg) + (b - pb) * (b - pb);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestColor = paletteColor;
            }
        }

        result[i] = closestColor;
    }

    return result;
}

std::vector<uint32_t> ColorReducer::kMeans(const std::vector<uint32_t> &image, int targetColors)
{
    std::vector<Color> pixels;
    pixels.reserve(image.size());
    for (uint32_t pixel : image)
    {
        pixels.emplace_back(pixel);
    }

    std::vector<Color> centroids;
    centroids.reserve(targetColors);

    // Initialize centroids randomly
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, image.size() - 1);
    for (int i = 0; i < targetColors; ++i)
    {
        centroids.push_back(pixels[dis(gen)]);
    }

    std::vector<int> assignments(pixels.size());
    bool changed = true;
    int maxIterations = 100;

    while (changed && maxIterations > 0)
    {
        changed = false;

        for (size_t i = 0; i < pixels.size(); ++i)
        {
            int nearestCentroid = 0;
            int minDistance = std::numeric_limits<int>::max();

            for (int j = 0; j < targetColors; ++j)
            {
                int distance = colorDistance(pixels[i], centroids[j]);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    nearestCentroid = j;
                }
            }

            if (assignments[i] != nearestCentroid)
            {
                assignments[i] = nearestCentroid;
                changed = true;
            }
        }

        std::vector<int> counts(targetColors, 0);
        std::vector<Color> newCentroids(targetColors);

        for (size_t i = 0; i < pixels.size(); ++i)
        {
            int centroidIndex = assignments[i];
            newCentroids[centroidIndex].r += pixels[i].r;
            newCentroids[centroidIndex].g += pixels[i].g;
            newCentroids[centroidIndex].b += pixels[i].b;
            counts[centroidIndex]++;
        }

        for (int i = 0; i < targetColors; ++i)
        {
            if (counts[i] > 0)
            {
                newCentroids[i].r /= counts[i];
                newCentroids[i].g /= counts[i];
                newCentroids[i].b /= counts[i];
            }
        }

        centroids = newCentroids;
        maxIterations--;
    }

    std::vector<uint32_t> result(image.size());
    for (size_t i = 0; i < pixels.size(); ++i)
    {
        result[i] = centroids[assignments[i]].toUint32();
    }

    return result;
}

std::vector<uint32_t> ColorReducer::octreeQuantization(const std::vector<uint32_t> &image, int targetColors)
{
    OctreeNode root;
    int leafCount = 0;
    int maxLevel = 8;

    for (uint32_t color : image)
    {
        root.addColor(color, 0, maxLevel, leafCount, targetColors);
    }

    std::vector<uint32_t> palette;
    std::queue<OctreeNode *> queue;
    queue.push(&root);

    while (!queue.empty() && palette.size() < static_cast<size_t>(targetColors))
    {
        OctreeNode *node = queue.front();
        queue.pop();

        if (node->isLeaf)
        {
            palette.push_back(node->getColor());
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                if (node->children[i] != nullptr)
                {
                    queue.push(node->children[i]);
                }
            }
        }
    }

    std::vector<uint32_t> result(image.size());
    for (size_t i = 0; i < image.size(); ++i)
    {
        uint32_t originalColor = image[i];
        uint32_t nearestColor = palette[0];
        int minDistance = std::numeric_limits<int>::max();

        for (uint32_t paletteColor : palette)
        {
            int distance = colorDistance(Color(originalColor), Color(paletteColor));
            if (distance < minDistance)
            {
                minDistance = distance;
                nearestColor = paletteColor;
            }
        }

        result[i] = nearestColor;
    }

    return result;
}