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
    /*
    float vertices[] = 
    {
        // first triangle
        0.5f, 0.5f, 0.0f,       // top right
        0.5f, -0.5f, 0.0f,      // bottom right
        -0.5f, 0.5f, 0.0f,      // top left
        // second triangle
        0.5f, -0.5f, 0.0f,       // bottom right
        -0.5f, -0.5f, 0.0f,      // bottom right
        -0.5f, 0.5f, 0.0f,       // top left
    };
    */
    float vertices[] = 
    {
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
    };

    uint32_t indices[] = 
    {
        0, 1, 3,        // first triangle
        1, 2, 3,        // second triangle
    };

    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    // GL Buffer를 생성, ARRAY_BUFFER 용도로 바인딩
    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*6, indices, GL_STATIC_DRAW);

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
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    // element buffer를 사용할 경우에 사용
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}