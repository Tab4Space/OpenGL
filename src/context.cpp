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
    m_box = Mesh::CreateBox();

    // shader 로딩 후 programe으로 만들기
    m_simpleProgram = Program::Create("./shader/simple.vs", "./shader/simple.fs");
    if(!m_simpleProgram)
    {
        return false;
    }
    m_program = Program::Create("./shader/lighting.vs", "./shader/lighting.fs");
    if(!m_program)
    {
        return false;
    }

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

    // 단색 texture
    TexturePtr darkGrayTexture = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());
    TexturePtr grayTexture = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./image/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

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
            m_cameraPitch = -20.0f;
            m_cameraPos = glm::vec3(0.0f, 2.5f, 8.0f);
        }

        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen)) 
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.5f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
        }

        ImGui::Checkbox("animation", &m_animation);
    }
    ImGui::End();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        // 동차좌표계를 쓰지만, 마지막에 1을 넣으면 점/0을 넣으면 벡터라는 의미이다. 방향 벡터임을 의미
        // 0을 넣으면 생기는 효과 > 평행이동이 안 된다
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // 핸드 라이트 효과
    // m_light.position = m_cameraPos;
    // m_light.direction = m_cameraFront;

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 30.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos+m_cameraFront, m_cameraUp);

    glm::vec3 lightPos = m_light.position;
    glm::vec3 lightDir = m_light.direction;
    if(m_flashLightMode)
    {
       lightPos = m_cameraPos;
       lightDir = m_cameraFront; 
    }
    else
    {
        /* light 위치에 박스를 그림 */
        //빛의 위치와 크기를 위한 선형변환 식, after computing projection and view matrix
        auto lightModelTransform =
            glm::translate(glm::mat4(1.0), m_light.position) *
            glm::scale(glm::mat4(1.0), glm::vec3(0.1f));

        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        m_box->Draw(m_simpleProgram.get());
        /* light 위치에 박스를 그림 */
    }

    /* spot light */
    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", lightPos);
    m_program->SetUniform("light.direction", lightDir);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0]+m_light.cutoff[1]))
    ));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);
    /* spot light end*/

    /* plane */
    auto modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));
    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    /* box1 */
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // stencil test 활성화 > op 설정 > always 설정 > mask 설정
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // replace는 아래 1 값과 대체된다 > 그림이 그려지는 부분은 1로 채워진
    glStencilFunc(GL_ALWAYS, 1, 0xFF);          // always니까 stencil test는 무조건 통과
    glStencilMask(0xFF);                        // 

    /* box2 */
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);        // 1이 아닌 지점만 그림을 그린다
    glStencilMask(0x00);                        // 스텐실 테스트가 통과되도 스텐실 버퍼 값을 업데이트하지 못한다
    glDisable(GL_DEPTH_TEST);                   // depth test 비활성화
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    m_simpleProgram->SetUniform("transform", transform*glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
    m_box->Draw(m_simpleProgram.get());
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
}
