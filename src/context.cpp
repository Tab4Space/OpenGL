#include "context.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if(!context->Init())
    {
        return nullptr;
    }
    return std::move(context);
}

bool Context::Init()
{
    float vertices[] = 
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // GL Buffer를 생성, ARRAY_BUFFER 용도로 바인딩
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);

    if(!vertShader || !fragShader)
    {
        return false;
    }
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    m_program = Program::Create({fragShader, vertShader});
    if(!m_program)
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", m_program->Get());

    // color setting for clearing
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program->Get());
    glDrawArrays(GL_LINE_STRIP, 0, 3);
}