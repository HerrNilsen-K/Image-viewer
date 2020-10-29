#include "texture.hpp"
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

    int Texture::getWidth() const { return this->m_width; }
    int Texture::getHeight() const { return this->m_height; }

} // namespace IViewer
