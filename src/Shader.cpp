#include "Renderer.h"
#include "Shader.h"
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

Shader::Shader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath) {
    std::string vertexShaderSource = ParseShader(vertexShaderFilePath);
    std::string fragmentShaderSource = ParseShader(fragmentShaderFilePath);
    m_RendererID = CreateShader(vertexShaderSource, fragmentShaderSource);
    if (m_RendererID == 0) {
        std::cerr << "Shader creation failed" << std::endl;
        glfwTerminate();
    }
}

Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const {
    glUseProgram(m_RendererID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

int Shader::GetUniformLocation(const std::string& name) {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }
    int location;
    GLCall(location = glGetUniformLocation(m_RendererID, name.c_str()));
    return location;
}

void Shader::SetUniform1ui(const std::string& name, unsigned int value) {
    int location = GetUniformLocation(name);
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist in 1ui!" << std::endl;
    }
    GLCall(glUniform1ui(location, value));
}

void Shader::SetUniform1i(const std::string& name, int value) {
    int location = GetUniformLocation(name);
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist in 1i!" << std::endl;
    }
    GLCall(glUniform1i(location, value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    int location;
    GLCall(location = GetUniformLocation(name));
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist in 4f!" << std::endl;
    }
    GLCall(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix) {
    int location;
    GLCall(location = GetUniformLocation(name));
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist in mat4f!" << std::endl;
    }
    GLCall(glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]));
}

std::string Shader::ParseShader(const std::string& filePath) {
    std::ifstream stream(filePath);
    std::string line;
    std::stringstream ss;
    while (getline(stream, line)) {
        ss << line << "\n";
    }
    return ss.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> message(length);
        glGetShaderInfoLog(id, length, &length, message.data());
        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
        std::cerr << message.data() << std::endl;
        glDeleteShader(id);
        return 0;
    }
    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program;
    GLCall(program = glCreateProgram());
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));
    return program;
}