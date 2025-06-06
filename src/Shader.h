#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

class Shader 
{
private:
    unsigned int m_RendererID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    std::string ParseShader(const std::string& filePath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
    int GetUniformLocation(const std::string& name);
public:
    Shader();
    Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);
    Shader(const Shader&) = delete; // cannot copy a shader
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept; // but can move
    Shader& operator=(Shader&& other) noexcept;
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniform1i(const std::string& name, int value);
    void SetUniform1ui(const std::string& name, unsigned int value);
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);

};