#include "context.h"
#include "image.h"
#include <imgui.h>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if(!context->Init())
    {
        return nullptr;
    }
    return std::move(context);
}

void Context::ProcessInput(GLFWwindow* window)
{
    if(!m_cameraControl)
    {
        return;
    }

    const float cameraSpeed = 0.05f;
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_cameraPos += cameraSpeed * m_cameraFront;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_cameraPos -= cameraSpeed * m_cameraFront;
    }

    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraFront));
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_cameraPos += cameraSpeed * cameraRight;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_cameraPos -= cameraSpeed * cameraRight;
    }

    auto cameraUp = glm::normalize(glm::cross(-m_cameraFront, cameraRight));
    if(glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        m_cameraPos += cameraSpeed * cameraUp;
    }
    if(glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        m_cameraPos -= cameraSpeed * cameraUp;
    }
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Context::MouseMove(double x, double y)
{
    if(!m_cameraControl)
    {
        return;
    }

    auto pos = glm::vec2((float)x, (float)y);
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.08f;
    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    // 0 ~ 360도 사이로 유지되도록
    if(m_cameraYaw < 0.0f)
    {
        m_cameraYaw += 360.0f;
    }
    if(m_cameraYaw > 360.0f)
    {
        m_cameraYaw -= 360.0f;
    }

    // 0 ~ 90도 사이로 유지되도록
    if(m_cameraPitch > 89.0f)
    {
        m_cameraPitch = 89.0f;
    }
    if(m_cameraPitch < -89.0f)
    {
        m_cameraPitch = -89.0f;
    }

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if(button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if(action == GLFW_PRESS)
        {
            m_prevMousePos = glm::vec2((float)x, (float)y);
            m_cameraControl = true;
        }
        else if(action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}

bool Context::Init()
{
    float vertices[] = { // pos.xyz, normal.xyz, texcoord.uv
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    // vertex_layout 클래스 사용
    m_vertexLayout = VertexLayout::Create();

    // buffer 클래스 사용
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float)*8*6*4);

    // m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, 0);
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, 0);                    // position
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, sizeof(float)*3);      // normal
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, sizeof(float)*6);      // texture coordination

    // buffer 클래스 사용
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t)*36);

    ShaderPtr vertShader = Shader::CreateFromFile("./shader/lighting.vs", GL_VERTEX_SHADER);
    ShaderPtr fragShader = Shader::CreateFromFile("./shader/lighting.fs", GL_FRAGMENT_SHADER);

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

    // color setting for clearing
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // Load image
    auto image = Image::Load("./image/container.jpg");
    if(!image)
    {
        return false;
    }
    SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // create check image
    // auto image = Image::Create(512, 512);
    // image->SetCheckImage(16, 16);

    // Use texture class
    m_texture = Texture::CreateFromImage(image.get());
    
    auto image2 = Image::Load("./image/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    // set texture slot
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    // Set Texture
    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    return true;
}

void Context::Render()
{
    // Render 함수는 1/60초 한번씩 실행이 된다

    if(ImGui::Begin("ui window"))
    {
        if(ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
        {
            // color가 변경되었을 경우 if 안의 로직이 실행된다
            glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
        }
        ImGui::Separator();     // ui를 분리해준다
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f, -89.0f, 89.0f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89.0f, 89.0f);
        ImGui::Separator();     // ui를 분리해준다
        
        if(ImGui::Button("reset camera"))
        {
            m_cameraYaw = 0.0f;
            m_cameraPitch = 0.0f;
            m_cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
        }

        if(ImGui::CollapsingHeader("light"))        
        {
            ImGui::ColorEdit3("light color", glm::value_ptr(m_lightColor));
            ImGui::ColorEdit3("object color", glm::value_ptr(m_objectColor));
            ImGui::SliderFloat("ambient strength", &m_ambientStrength, 0.0f, 1.0f);
        }
    }
    ImGui::End();

    m_program->Use();
    m_program->SetUniform("lightColor", m_lightColor);
    m_program->SetUniform("objectColor", m_objectColor);
    m_program->SetUniform("ambientStrength", m_ambientStrength);

    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        // 동차좌표계를 쓰지만, 마지막에 1을 넣으면 점/0을 넣으면 벡터라는 의미이다. 방향 벡터임을 의미
        // 0을 넣으면 생기는 효과 > 평행이동이 안 된다
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 30.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos+m_cameraFront, m_cameraUp);

    for (size_t i = 0; i<cubePositions.size(); i++)
    {
        auto& pos = cubePositions[i];
        auto model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::rotate(model, glm::radians((float)glfwGetTime()*120.0f+20.0f*(float)i), glm::vec3(1.0f, 0.5f, 0.0f));
        auto transform = projection * view * model;
        m_program->SetUniform("transform", transform);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
}
