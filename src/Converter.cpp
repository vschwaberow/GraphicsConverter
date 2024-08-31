#include "Converter.h"

void Converter::convertKoalaToPNG(const char *inputFile, const char *outputFile)
{
    std::cout << "Converting Koala to PNG: " << inputFile << " -> " << outputFile << std::endl;
}

void Converter::convertPNGToKoala(const char *inputFile, const char *outputFile)
{
    std::cout << "Converting PNG to Koala: " << inputFile << " -> " << outputFile << std::endl;
}

std::vector<uint8_t> Converter::convertToBitmap(const std::vector<uint32_t> &inputImage, int width, int height)
{
    if (inputImage.empty() || width <= 0 || height <= 0)
    {
        throw std::invalid_argument("Invalid input parameters");
    }
    return std::vector<uint8_t>(width * height / 8);
}

std::vector<uint8_t> Converter::convertToHires(const std::vector<uint32_t> &inputImage, int width, int height)
{
    if (inputImage.empty() || width <= 0 || height <= 0)
    {
        throw std::invalid_argument("Invalid input parameters");
    }
    return std::vector<uint8_t>(width * height / 8);
}

std::vector<uint8_t> Converter::convertToMulticolor(const std::vector<uint32_t> &inputImage, int width, int height)
{
    if (inputImage.empty() || width <= 0 || height <= 0)
    {
        throw std::invalid_argument("Invalid input parameters");
    }
    return std::vector<uint8_t>(width * height / 8);
}
