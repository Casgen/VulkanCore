#include <cstdint>
#include <fstream>
#include <stdexcept>

#include "FileUtils.h"
#include "stb_image.h"

std::vector<char> FileUtils::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        throw std::runtime_error("Failed to open file!");

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkCore::ImageData FileUtils::ReadImage(const std::string& filename, unsigned int desiredChannels)
{
    return ReadImage(filename.data());
}

VkCore::ImageData FileUtils::ReadImage(const char* filename, unsigned int desiredChannels)
{
    VkCore::ImageData imageData;

    int height, width, channels;

    imageData.data = stbi_load(filename, &width, &height, &channels, desiredChannels);

    imageData.width = static_cast<uint32_t>(width);
    imageData.height = static_cast<uint32_t>(height);
    imageData.channels = static_cast<uint32_t>(channels);

    if (imageData.data == nullptr) {
        throw std::runtime_error("Failed to load an image! File doesn't exist or is corrupted!");
    }

    imageData.dataSize = imageData.width * imageData.height * imageData.channels;

    return imageData;
}
