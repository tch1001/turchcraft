#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const std::string& path)
    : m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr), 
      m_Width(0), m_Height(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);

    GLCall(glGenTextures(1, &m_RendererID));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
    GLCall(glDeleteTextures(1, &m_RendererID));
}

// Implement move constructor
Texture::Texture(Texture&& other) noexcept
    : m_RendererID(other.m_RendererID), m_FilePath(std::move(other.m_FilePath)), 
      m_LocalBuffer(other.m_LocalBuffer), m_Width(other.m_Width), 
      m_Height(other.m_Height), m_BPP(other.m_BPP)
{
    other.m_RendererID = 0;
    other.m_LocalBuffer = nullptr;
    other.m_Width = 0;
    other.m_Height = 0;
    other.m_BPP = 0;
}

// Implement move assignment operator
Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        GLCall(glDeleteTextures(1, &m_RendererID));

        m_RendererID = other.m_RendererID;
        m_FilePath = std::move(other.m_FilePath);
        m_LocalBuffer = other.m_LocalBuffer;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        m_BPP = other.m_BPP;

        other.m_RendererID = 0;
        other.m_LocalBuffer = nullptr;
        other.m_Width = 0;
        other.m_Height = 0;
        other.m_BPP = 0;
    }
    return *this;
}

void Texture::Bind(unsigned int slot) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
