
#include "ImageData.h"

namespace VkCore
{

    ImageData::ImageData(ImageData&& other)
    {

        if (this != &other)
        {
            width = other.width;
            height = other.height;
            channels = other.channels;

            data = other.data;

            other.width = 0;
            other.height = 0;
            other.channels = 0;

            other.data = nullptr;
        }
    }

    ImageData& ImageData::operator=(ImageData&& other)
    {

        if (this != &other)
        {
            width = other.width;
            height = other.height;
            channels = other.channels;

            data = other.data;

            other.width = 0;
            other.height = 0;
            other.channels = 0;

            other.data = nullptr;
        }

        return *this;
    }

    ImageData::~ImageData()
    {
        stbi_image_free(data);
    }

    void ImageData::Free()
    {
        if (data != nullptr)
        {
            stbi_image_free(data);
            data = nullptr;
        }
    }
} // namespace VkCore
