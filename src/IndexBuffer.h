#pragma once

class IndexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_Count;
public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer &operator=(const IndexBuffer&) = delete;

    IndexBuffer(IndexBuffer &&other) noexcept;

    IndexBuffer &operator=(IndexBuffer &&other) noexcept;

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const { return m_Count; }
}; 