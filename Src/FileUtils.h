#pragma once

#include <vector>
#include <string>

#include "Vk/Texture/ImageData.h"
#include "stb_image.h"


class FileUtils
{

public:
    static std::vector<char> ReadFile(const std::string& filename);

    static VkCore::ImageData ReadImage(const std::string& filename, unsigned int desiredChannels = STBI_rgb_alpha);

    static VkCore::ImageData ReadImage(const char* filename, unsigned int desiredChannels = STBI_rgb_alpha);
    
};
