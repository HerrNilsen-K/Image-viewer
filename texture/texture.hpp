#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

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

        int getWidth() const { return this->m_width; }
        int getHeight() const { return this->m_height; }
    };
} // namespace IViewer

#endif // __TEXTURE_H__