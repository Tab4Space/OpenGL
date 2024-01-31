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

    // 화면과 동인한 크기의 frame buffer를 생성 > 이후에 buffer에 scene을 렌더링할 것
    m_framebuffer = Framebuffer::Create({Texture::Create(width, height, GL_RGBA)});

    // deferred shading을 위한 framebuffer 준비, 3장의 texture를 넣는다
    m_deferGeoFramebuffer = Framebuffer::Create({
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),           // position
        Texture::Create(width, height, GL_RGBA16F, GL_FLOAT),           // normal
        Texture::Create(width, height, GL_RGBA, GL_UNSIGNED_BYTE),      // albedo-spec
    });
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

void Context::DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program)
{
    program->Use();
    auto modelTransform = 
        glm::translate(glm::mat4(1.0), glm::vec3(0.0f, -0.5f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(40.0f, 1.0f, 40.f));
    auto transform = projection * view * modelTransform;

    /* plane */
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(program);
    m_box->Draw(program);
    /* plane */


    /* box1 */
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box1Material->SetToProgram(program);
    m_box->Draw(program);

    /* box2 */
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);
    /* box2 */
    
    /* box 3*/
    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 1.75f, -2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    transform = projection * view * modelTransform;
    program->SetUniform("transform", transform);
    program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(program);
    m_box->Draw(program);
    /* box 3*/
}

bool Context::Init()
{
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);

    m_box = Mesh::CreateBox();
    m_plane = Mesh::CreatePlane();

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

    m_textureProgram = Program::Create("./shader/texture.vs", "./shader/texture.fs");
    if(!m_textureProgram)
    {
        return false;
    }

    m_postProgram = Program::Create("./shader/texture.vs", "./shader/gamma.fs");
    if(!m_postProgram)
    {
        return false;
    }

    // color setting for clearing
    // glClearColor(0.1f, 0.2f, 0.3f, 0.0f);

    // 단색 texture
    TexturePtr darkGrayTexture = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());
    TexturePtr grayTexture = Texture::CreateFromImage(Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(Image::Load("./image/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 32.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(Image::Load("./image/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(Image::Load("./image/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

    m_windowTexture = Texture::CreateFromImage(Image::Load("./image/blending_transparent_window.png").get());

    // 이미지 로딩
    auto cubeRight = Image::Load("./image/skybox/right.jpg", false);
    auto cubeLeft = Image::Load("./image/skybox/left.jpg", false);
    auto cubeTop = Image::Load("./image/skybox/top.jpg", false);
    auto cubeBottom = Image::Load("./image/skybox/bottom.jpg", false);
    auto cubeFront = Image::Load("./image/skybox/front.jpg", false);
    auto cubeBack = Image::Load("./image/skybox/back.jpg", false);

    m_cubeTexture = CubeTexture::CreateFromImages({
        cubeRight.get(),
        cubeLeft.get(),
        cubeTop.get(),
        cubeBottom.get(),
        cubeFront.get(),
        cubeBack.get(),
    });

    // skybox, env shader
    m_skyboxProgram = Program::Create("./shader/skybox.vs", "./shader/skybox.fs");
    m_envMapProgram = Program::Create("./shader/env_map.vs", "./shader/env_map.fs");

    // grass shader
    m_grassTexture = Texture::CreateFromImage(Image::Load("./image/grass.png").get());
    m_grassProgram = Program::Create("./shader/grass.vs", "./shader/grass.fs");
    m_grassPos.resize(100000);      // 테스트를 위해 10만개로
    for(size_t i = 0; i < m_grassPos.size(); i++) 
    {
        m_grassPos[i].x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
        m_grassPos[i].z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
        m_grassPos[i].y = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);       // 회전까지
    }

    /* instancing
    grassInstance는 VAO이고 이미 만들어져있던 plane 메쉬에서 사용하던 vertex buffer와 index buffer를 재활용
    재활용 + 새로 만든 grassPos 값을 담은 array buffer를 attribute 3번으로 세팅하고 instance 별로 값이 들어오도록 세팅
    */

    // VAO 만들고 plane 오브젝트에 바인딩 -> 0(position), 1(normal), 2(texture) attribute 세팅
    m_grassInstance = VertexLayout::Create();
    m_grassInstance->Bind();
    m_plane->GetVertexBuffer()->Bind();
    m_grassInstance->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    m_grassInstance->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal));
    m_grassInstance->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord));

    // buffer를 새로 생성 -> m_grassInstance의 vertex layout에 바인딩 -> 3번 attribute에 세팅
    // glVertexAttribDivisor(3, 1): 3번 attribute는 instance가 바뀔때마다 값이 바뀌도록 한다
    // grassInstance는 plane에 있는 IndexBuffer를 사용하기 위해 바인딩
    m_grassPosBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, m_grassPos.data(), sizeof(glm::vec3), m_grassPos.size());
    m_grassPosBuffer->Bind();
    m_grassInstance->SetAttrib(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glVertexAttribDivisor(3, 1);
    m_plane->GetIndexBuffer()->Bind();

    /* shadow map */
    m_shadowMap = ShadowMap::Create(1024, 1024);
    m_lightingShadowProgram = Program::Create("./shader/lighting_shadow.vs", "./shader/lighting_shadow.fs");
    /* shadow map end */

    /* normal map */
    m_brickDiffuseTexture = Texture::CreateFromImage(Image::Load("./image/brickwall.jpg", false).get());
    m_brickNormalTexture = Texture::CreateFromImage(Image::Load("./image/brickwall_normal.jpg", false).get());
    m_normalProgram = Program::Create("./shader/normal.vs", "./shader/normal.fs");
    /* normal map end */

    /* deferred shading */
    m_deferGeoProgram = Program::Create("./shader/defer_geo.vs", "./shader/defer_geo.fs");
    /* deferred shading end */

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
        ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
        ImGui::Separator();     // ui를 분리해준다
        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
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
            ImGui::Checkbox("l.directional", &m_light.directional);
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.1f, 0.0f, 180.0f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.5f, 0.0f, 1000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
            ImGui::Checkbox("flash light", &m_flashLightMode);
            ImGui::Checkbox("l.blinn", &m_blinn);
        }

        ImGui::Checkbox("animation", &m_animation);

        // float aspecRatio = m_width / m_height;
        // ImGui::Image((ImTextureID)m_framebuffer->GetColorAttachment()->Get(), ImVec2(150 * aspecRatio, 150));

        // shadow map imgui에 그리기
        ImGui::Image((ImTextureID)m_shadowMap->GetShadowMap()->Get(), ImVec2(256,256), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    if (ImGui::Begin("G-Buffers")) 
    {
        const char* bufferNames[] = {"position", "normal", "albedo/specular",};
        static int bufferSelect = 0;
        
        ImGui::Combo("buffer", &bufferSelect, bufferNames, 3);
        
        float width = ImGui::GetContentRegionAvail().x;
        float height = width * ((float)m_height / (float)m_width);
        auto selectedAttachment = m_deferGeoFramebuffer->GetColorAttachment(bufferSelect);
        ImGui::Image((ImTextureID)selectedAttachment->Get(),
        ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::End();

    m_cameraFront = 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) * 
        // 동차좌표계를 쓰지만, 마지막에 1을 넣으면 점/0을 넣으면 벡터라는 의미이다. 방향 벡터임을 의미
        // 0을 넣으면 생기는 효과 > 평행이동이 안 된다
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    // 핸드 라이트 효과
    // m_light.position = m_cameraPos;
    // m_light.direction = m_cameraFront;

    auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.1f, 150.0f);
    auto view = glm::lookAt(m_cameraPos, m_cameraPos+m_cameraFront, m_cameraUp);

    /* shadow map, 다른 것이 그려지기 전에 먼저 shadow map이 그려져야 한다 */
    // shadow map이 갖고 있는 depth buffer의 depth 값을 렌더링
    // 여기가 첫 번째 단계
    auto lightView = glm::lookAt(m_light.position, m_light.position + m_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
    auto lightProjection = m_light.directional ?
        glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f) : 
        glm::perspective(glm::radians((m_light.cutoff[0] + m_light.cutoff[1]) * 2.0f), 1.0f, 1.0f, 20.0f);

    // frame buffe binding
    m_shadowMap->Bind();
    glClear(GL_DEPTH_BUFFER_BIT);
    // frame buffer의 사이즈대로 세팅
    glViewport(0, 0, m_shadowMap->GetShadowMap()->GetWidth(), m_shadowMap->GetShadowMap()->GetHeight());
    m_simpleProgram->Use();
    m_simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    DrawScene(lightView, lightProjection, m_simpleProgram.get());

    // deffered shading first pass rendering
    m_deferGeoFramebuffer->Bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, m_width, m_height);
    m_deferGeoProgram->Use();
    DrawScene(view, projection, m_deferGeoProgram.get());

    // 원상복구
    Framebuffer::BindToDefault();
    glViewport(0, 0, m_width, m_height);
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    /* shadow map end */

    // 아래에 나오는 그림을 그리는 코드들이 우리가 만든 프레임 버퍼에 그림을 그린다
    // m_framebuffer->Bind();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // skybox 그리기
    auto skyboxModelTransform = glm::translate(glm::mat4(1.0), m_cameraPos) * glm::scale(glm::mat4(1.0), glm::vec3(50.0f));
    m_skyboxProgram->Use();
    m_cubeTexture->Bind();
    m_skyboxProgram->SetUniform("skybox", 0);
    m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    m_box->Draw(m_skyboxProgram.get());

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

    /* shadow map second pass */
    // 기존의 light 대체
    m_lightingShadowProgram->Use();
    m_lightingShadowProgram->SetUniform("viewPos", m_cameraPos);
    m_lightingShadowProgram->SetUniform("light.directional", m_light.directional ? 1 : 0);
    m_lightingShadowProgram->SetUniform("light.position", m_light.position);
    m_lightingShadowProgram->SetUniform("light.direction", m_light.direction);
    m_lightingShadowProgram->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_lightingShadowProgram->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    m_lightingShadowProgram->SetUniform("light.ambient", m_light.ambient);
    m_lightingShadowProgram->SetUniform("light.diffuse", m_light.diffuse);
    m_lightingShadowProgram->SetUniform("light.specular", m_light.specular);
    m_lightingShadowProgram->SetUniform("blinn", (m_blinn ? 1 : 0));
    m_lightingShadowProgram->SetUniform("lightTransform", lightProjection * lightView);
    // texture 3번 슬롯 활성화
    glActiveTexture(GL_TEXTURE3);
    m_shadowMap->GetShadowMap()->Bind();
    // shadow map에 3번 슬롯 할당
    m_lightingShadowProgram->SetUniform("shadowMap", 3);
    glActiveTexture(GL_TEXTURE0);

    DrawScene(view, projection, m_lightingShadowProgram.get());
    /* shadow map second pass end */

    /* normal map */
    // 일단 그림자랑 상관없이 그린다
    auto modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0));
    m_normalProgram->Use();
    m_normalProgram->SetUniform("viewPos", m_cameraPos);
    m_normalProgram->SetUniform("lightPos", m_light.position);
    
    glActiveTexture(GL_TEXTURE0);
    m_brickDiffuseTexture->Bind();
    m_normalProgram->SetUniform("diffuse", 0);

    glActiveTexture(GL_TEXTURE1);
    m_brickNormalTexture->Bind();
    m_normalProgram->SetUniform("normalMal", 1);

    glActiveTexture(GL_TEXTURE0);
    m_normalProgram->SetUniform("modelTransform", modelTransform);
    m_normalProgram->SetUniform("transform", projection * view * modelTransform);
    m_plane->Draw(m_normalProgram.get());
    /* normal map end */

    /* env map box */
    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.75f, -2.0f)) *
    //     glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    //     glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
    // m_envMapProgram->Use();
    // m_envMapProgram->SetUniform("model", modelTransform);
    // m_envMapProgram->SetUniform("view", view);
    // m_envMapProgram->SetUniform("projection", projection);
    // m_envMapProgram->SetUniform("cameraPos", m_cameraPos);
    // m_cubeTexture->Bind();
    // m_envMapProgram->SetUniform("skybox", 0);
    // m_box->Draw(m_envMapProgram.get());
    // /* env map box end */

    /* blend test */
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // // 29번 강의를 위해 주석처리
    // //glEnable(GL_CULL_FACE);
    // //glCullFace(GL_BACK);

    // m_textureProgram->Use();
    // m_windowTexture->Bind();
    // m_textureProgram->SetUniform("tex", 0);

    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());

    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());

    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
    // transform = projection * view * modelTransform;
    // m_textureProgram->SetUniform("transform", transform);
    // m_plane->Draw(m_textureProgram.get());
    /* blend test end */

    // glEnable(GL_BLEND);
    // glDisable(GL_CULL_FACE);

    // m_grassProgram->Use();
    // m_grassProgram->SetUniform("tex", 0);
    // m_grassTexture->Bind();
    // // 그림을 그리기 전에 VAO인 m_grassInstance를 바인딩 -> transform 구하기 -> glDrawElementsInstanced 호출
    // m_grassInstance->Bind();
    // modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    // transform = projection * view * modelTransform;
    // m_grassProgram->SetUniform("transform", transform);
    // // 렉이 걸렸던 첫 번째 방법(10만개 테스트)과 다르게 렉이 안 걸린다
    // glDrawElementsInstanced(GL_TRIANGLES, m_plane->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0, m_grassPosBuffer->GetCount());
    
    // 디폴트 화면으로 그림이 그려질 대상을 변경
    // Framebuffer::BindToDefault();
    // // 디폴트 화면도 clear
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // // 텍스처 프로그램을 활성화 > frame buffer 안에 있는 텍스처를 바인딩
    // m_postProgram->Use();
    // m_postProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f)));
    // m_framebuffer->GetColorAttachment()->Bind();
    // m_postProgram->SetUniform("tex", 0);
    // m_postProgram->SetUniform("gamma", m_gamma);
    // m_plane->Draw(m_postProgram.get());
}
