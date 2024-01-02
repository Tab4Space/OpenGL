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

    // vertex_layout 클래스 사용
    m_vertexLayout = VertexLayout::Create();


    // buffer 클래스 사용
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float)*12);

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);

    // buffer 클래스 사용
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t)*6);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);

    if(!vertShader || !fragShader)
    {
        return false;
    }
    SPDLOG_INFO("vertex shader id: {}", vertShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragShader->Get());

    // shader를 attach해서 link
    m_program = Program::Create({fragShader, vertShader});
    if(!m_program)
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", m_program->Get());

    // shader의 uniform에 값을 세팅
    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    // color setting for clearing
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float time = 0.0f;
    float t = sinf(time) * 0.5f + 0.5f;
    auto loc = glGetUniformLocation(m_program->Get(), "color");;
    m_program->Use();
    glUniform4f(loc, t*t, 2.0f*t*(1.0f-t), (1.0f-t)*(1.0f-t), 1.0f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    time += 0.016f;

    // m_program->Use();
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}