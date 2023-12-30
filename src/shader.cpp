#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType)
{
    auto shader = ShaderUPtr(new Shader());
     if(!shader->LoadFile(filename, shaderType))
     {
        return nullptr;
     }
     return std::move(shader);
}

Shader::~Shader()
{
    if(m_shader)
    {
        glDeleteShader(m_shader);
    }
}

bool Shader::LoadFile(const std::string& filename, GLenum shaderType)
{
    auto result = LoadTextFile(filename);
    // optional의 리턴은 있을수도, 없을 수도 따라서 has_value()로 체크
    if(!result.has_value())
    {
        return false;
    }

    // result 안의 string이 크다면? > &없이 사용하면 memory copy 식으로 동작
    // auto가 & 를 붙일지 말지는 추론하지 못함 > 따라서 명시적으로 붙여줘야 한다
    auto& code = result.value();
    const char* codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();

    // create and compile shader
    m_shader = glCreateShader(shaderType);
    glShaderSource(m_shader, 1, (const GLchar* const*)&codePtr, &codeLength);
    glCompileShader(m_shader);

    // check compile error
    int success = 0;
    // shader의 compile 정보를 얻는다
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char infoLog[1024];
        glGetShaderInfoLog(m_shader, 1024, nullptr, infoLog);
        SPDLOG_ERROR("failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}