#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#ifdef _WIN32
#include <stb_image.h>
#elif __linux__
#include <stb/stb_image.h>
#endif

#include <string>

namespace IViewer
{
    class Texture
    {
    private:
        unsigned char *m_imageData;
        int m_width, m_height, m_colorChannels;

    public:
        Texture(std::string c_filePath);
        Texture(const char *c_filePath);
        ~Texture();

        int getWidth() const;
        int getHeight() const;
    };
} // namespace IViewer

#endif // __TEXTURE_H__