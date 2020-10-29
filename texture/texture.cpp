#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#ifdef _WIN32
#include <stb_image.h>
#elif __linux__
#include <stb/stb_image.h>
#endif

#include <iostream>

namespace IViewer
{
    Texture::Texture(std::string c_filePath)
    {
        stbi_set_flip_vertically_on_load(1);
        unsigned char *imageData = stbi_load(c_filePath.c_str(), &m_width, &m_height, &m_colorChannels, 0);
        if (!imageData)
        {
            std::cout << "----------------------\n";
            std::cout << "Failed to laod texture!\n";
            std::cout << stbi_failure_reason() << std::endl;
            std::cout << "----------------------\n";
        }
    }

    Texture::Texture(const char *c_filePath)
    {
    }

    Texture::~Texture()
    {
    }

} // namespace IViewer
