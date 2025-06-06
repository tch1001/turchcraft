#pragma once

class VertexBuffer
{
private:
    unsigned int m_RendererID;
public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    VertexBuffer(VertexBuffer &&other) noexcept;

    VertexBuffer &operator=(VertexBuffer &&other) noexcept;

    void Bind() const;
    void Unbind() const;
};
