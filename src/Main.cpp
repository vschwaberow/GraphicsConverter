// SPDX-License-Identifier: MIT OR Apache-2.0
// Project: gfxconverter
// File: src/Main.cpp
// Author: Volker Schwaberow <volker@schwaberow.de>
// Copyright (c) 2022 Volker Schwaberow

#include "Main.h"

std::string openFileDialog()
{
    nfdchar_t *outPath = nullptr;
    nfdresult_t result = NFD_OpenDialog(
        "kla,png,jpg,gif",
        nullptr,
        &outPath);
    if (result == NFD_OKAY)
    {
        std::string selectedPath(outPath);
        free(outPath);
        return selectedPath;
    }
    return "";
}

std::string saveFileDialog(const std::string &defaultName)
{
    nfdchar_t *outPath = nullptr;

    std::string defaultDir = defaultName.substr(0, defaultName.find_last_of("/\\"));

    nfdresult_t result = NFD_SaveDialog(
        "kla",
        defaultDir.empty() ? nullptr : defaultDir.c_str(),
        &outPath);
    if (result == NFD_OKAY)
    {
        std::string selectedPath(outPath);
        free(outPath);
        return selectedPath;
    }
    return "";
}

void createConvertedTexture(const std::vector<uint32_t> &pixels, int width, int height)
{
    if (convertedTextureID != 0)
    {
        glDeleteTextures(1, &convertedTextureID);
    }

    glGenTextures(1, &convertedTextureID);
    glBindTexture(GL_TEXTURE_2D, convertedTextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glBindTexture(GL_TEXTURE_2D, 0);

    convertedImageWidth = width;
    convertedImageHeight = height;
}

bool loadImageFile(const std::string &filename)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "kla")
    {
        // TODO: Implement Koala image loading
        spdlog::warn("Koala image loading not yet implemented");
        return false;
    }
    else if (extension == "png" || extension == "jpg" || extension == "gif")
    {
        Image img = loadImage(filename.c_str());
        if (img.data != nullptr)
        {
            originalImage = img;
            imageLoaded = true;
            spdlog::info("Successfully loaded image: {}x{} pixels", img.width, img.height);
            glGenTextures(1, &originalTextureID);
            glBindTexture(GL_TEXTURE_2D, originalTextureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
            glBindTexture(GL_TEXTURE_2D, 0);

            originalImageWidth = img.width;
            originalImageHeight = img.height;

            return true;
        }
    }
    return false;
}

Image loadImage(const char *filename)
{
    Image img;
    img.data = stbi_load(filename, &img.width, &img.height, &img.channels, 4);

    if (img.data == nullptr)
    {
        // TODO: Add error handling
        return img;
    }

    glGenTextures(1, &img.textureID);
    glBindTexture(GL_TEXTURE_2D, img.textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width, img.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return img;
}

int main(int, char **)
{

    if (!glfwInit())
        return 1;

    Logger::initialize();
    guiSink = Logger::getGuiSink();

    GLFWwindow *window = glfwCreateWindow(1280, 720, "Graphics Converter", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    ColorReductionAlgorithm currentColorAlgo = ColorReductionAlgorithm::MedianCut;
    DitheringAlgorithm currentDitheringAlgo = DitheringAlgorithm::FloydSteinberg;
    int targetColors = 16;

    std::string loadedFilename;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Retro Graphics Converter");

        if (ImGui::Button("Load Image"))
        {
            std::string filename = openFileDialog();
            if (!filename.empty())
            {
                if (loadImageFile(filename))
                {
                    loadedFilename = filename;
                }
                else
                {
                    ImGui::OpenPopup("Error");
                }
            }
        }
        if (ImGui::BeginPopupModal("Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Failed to load the image file.");
            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Text("%s", loadedFilename.c_str());

        ImGui::BeginDisabled(!imageLoaded);

        if (ImGui::Button("Convert Koala to PNG"))
        {
            Converter::convertKoalaToPNG("input.kla", "output.png");
        }

        if (ImGui::Button("Convert PNG to Koala"))
        {
            Converter::convertPNGToKoala("input.png", "output.kla");
        }

        ImGui::Separator();
        ImGui::Text("Color Reduction");
        ImGui::Combo("Algorithm", (int *)&currentColorAlgo, "Median Cut\0K-Means\0Octree Quantization\0");
        ImGui::SliderInt("Target Colors", &targetColors, 2, 256);
        if (ImGui::Button("Apply Color Reduction"))
        {
            if (imageLoaded)
            {
                std::vector<uint32_t> imageData(originalImage.width * originalImage.height);
                for (int i = 0; i < originalImage.width * originalImage.height; ++i)
                {
                    imageData[i] = ((uint32_t *)originalImage.data)[i];
                }

                std::vector<uint32_t> reducedImage = ColorReducer::reduceColors(imageData, originalImage.width, originalImage.height, targetColors, currentColorAlgo);
                spdlog::debug("Called ColorReducer::reduceColors() with {} colors and algorithm {}", targetColors, ColorReducer::getColorReducerName(currentColorAlgo));
                createConvertedTexture(reducedImage, originalImage.width, originalImage.height);
                spdlog::debug("Reduced image to {} colors and has a size of {} bytes", targetColors, reducedImage.size() * sizeof(uint32_t));
                spdlog::info("Applied color reduction: {} colors", targetColors);
            }
        }

        ImGui::Separator();
        ImGui::Text("Dithering");
        ImGui::Combo("Dithering Algorithm", (int *)&currentDitheringAlgo, "Floyd-Steinberg\0Bayer\0Ordered\0");
        if (ImGui::Button("Apply Dithering"))
        {
            std::vector<uint32_t> imageData(originalImage.width * originalImage.height);
            for (int i = 0; i < originalImage.width * originalImage.height; ++i)
            {
                imageData[i] = ((uint32_t *)originalImage.data)[i];
            }

            std::vector<uint32_t> reducedImage = ColorReducer::reduceColors(imageData, originalImage.width, originalImage.height, targetColors, currentColorAlgo);

            std::vector<uint32_t> palette;
            for (uint32_t color : reducedImage)
            {
                if (std::find(palette.begin(), palette.end(), color) == palette.end())
                {
                    palette.push_back(color);
                }
                if (palette.size() >= static_cast<size_t>(targetColors))
                {
                    break;
                }
            }

            std::vector<uint32_t> ditheredImage = Dithering::applyDithering(imageData, originalImage.width, originalImage.height, palette, currentDitheringAlgo);

            createConvertedTexture(ditheredImage, originalImage.width, originalImage.height);

            spdlog::info("Applied dithering: {} algorithm with {} colors",
                         Dithering::getAlgorithmName(currentDitheringAlgo), targetColors);
        }
        ImGui::EndDisabled();
        if (ImGui::Button("Toggle Debug Window"))
        {
            showDebugWindow = !showDebugWindow;
        }

        ImGui::BeginDisabled(!imageLoaded);

        if (imageLoaded && originalTextureID != 0)
        {
            ImGui::Text("Original Image");
            ImGui::Image((void *)(intptr_t)originalTextureID, ImVec2(originalImageWidth, originalImageHeight));
        }

        if (convertedTextureID != 0)
        {
            ImGui::Text("Converted Image");
            ImGui::Image((void *)(intptr_t)convertedTextureID, ImVec2(convertedImageWidth, convertedImageHeight));
        }

        ImGui::EndDisabled();
        ImGui::End();

        if (showDebugWindow)
        {
            ImGui::Begin("Debug Window", &showDebugWindow);

            if (ImGui::Button("Log Debug Message"))
            {
                spdlog::debug("This is a debug message");
            }

            if (ImGui::Button("Log Info Message"))
            {
                spdlog::info("This is an info message");
            }

            if (ImGui::Button("Log Warning Message"))
            {
                spdlog::warn("This is a warning message");
            }

            if (ImGui::Button("Log Error Message"))
            {
                spdlog::error("This is an error message");
            }

            ImGui::Separator();

            ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
            for (const auto &logMsg : guiSink->getLogBuffer())
            {
                ImVec4 color;
                switch (logMsg.level)
                {
                case spdlog::level::trace:
                case spdlog::level::debug:
                    color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
                    break;
                case spdlog::level::info:
                    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case spdlog::level::warn:
                    color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                    break;
                case spdlog::level::err:
                case spdlog::level::critical:
                    color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    break;
                default:
                    color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                }
                ImGui::TextColored(color, "%s", logMsg.message.c_str());
            }
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();

            ImGui::End();
        }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    stbi_image_free(originalImage.data);
    stbi_image_free(processedImage.data);
    glDeleteTextures(1, &originalImage.textureID);
    glDeleteTextures(1, &processedImage.textureID);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}