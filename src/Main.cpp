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

bool loadImageFile(const std::string &filename)
{
    std::string extension = filename.substr(filename.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == "kla")
    {
        // TODO: Implement Koala image loading
        return false;
    }
    else if (extension == "png" || extension == "jpg" || extension == "gif")
    {
        Image img = loadImage(filename.c_str());
        if (img.data != nullptr)
        {
            originalImage = img;
            imageLoaded = true;
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
            convertKoalaToPNG("input.kla", "output.png");
        }

        if (ImGui::Button("Convert PNG to Koala"))
        {
            convertPNGToKoala("input.png", "output.kla");
        }

        ImGui::Separator();
        ImGui::Text("Color Reduction");
        ImGui::Combo("Algorithm", (int *)&currentColorAlgo, "Median Cut\0K-Means\0Octree Quantization\0");
        ImGui::SliderInt("Target Colors", &targetColors, 2, 256);
        if (ImGui::Button("Apply Color Reduction"))
        {
            // TODO: Implement color reduction
            // std::vector<uint32_t> reducedImage = ColorReducer::reduceColors(originalImage, width, height, targetColors, currentColorAlgo);
        }

        ImGui::Separator();
        ImGui::Text("Dithering");
        ImGui::Combo("Dithering Algorithm", (int *)&currentDitheringAlgo, "Floyd-Steinberg\0Bayer\0Ordered\0");
        if (ImGui::Button("Apply Dithering"))
        {
            // TODO: Implement dithering
            // std::vector<uint32_t> ditheredImage = Dithering::applyDithering(reducedImage, width, height, palette, currentDitheringAlgo);
        }

        ImGui::EndDisabled();
        ImGui::End();

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