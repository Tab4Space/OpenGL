#include "context.h"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


/* Callbacks */
void OnFrameBufferSizeChange(GLFWwindow* window, int width, int height)
{
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    auto context = reinterpret_cast<Context*>(glfwGetWindowUserPointer(window));    // c++ style type cast
    context->Reshape(width, height);
}

void OnKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods:{}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Release" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-"
    );
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y)
{
    auto context = (Context*)glfwGetWindowUserPointer(window);      // c style type cast
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);

    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow* window, unsigned int ch)
{
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}
/* Callbacks End */

int main(int argc, const char** argv)
{
    SPDLOG_INFO("Start program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력 후 종료
    SPDLOG_INFO("Initialize glfw");
    if(!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("Failed to initialize glfw: {}", description);
        return -1;
    }

    // glfw 윈도우를 생성하기 전에 만들기 희망하는 OpenGL 버전 추가
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);

    // glfw 윈도우 생성, 실패하면 에러 출력후 종료
    SPDLOG_INFO("Create glfw window");
    auto* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if(!window)
    {
        SPDLOG_ERROR("Failed to create glfw window");
        glfwTerminate();
        return -1;
    }
    // 사용할 context (opengl의 context)
    glfwMakeContextCurrent(window);

    // glad를 활용한 OpenGL 함수 로딩, 성공해야 이후부터 opengl 함수 사용 가능
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
    {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", (const char*)glVersion);

    // ImGui context 생성
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, false);        // callback setting을 직접 하기위해 false
    ImGui_ImplOpenGL3_Init();                           // opengl3의 renderer를 초기화
    ImGui_ImplOpenGL3_CreateFontsTexture();             // imgui에서 사용하는 font, program 초기화
    ImGui_ImplOpenGL3_CreateDeviceObjects();            // device 안에서 사용할 오브젝트 생성(texture, font 등)

    // 프로그램 내부에서 사용할 context 생성
    auto context = Context::Create();
    if(!context)
    {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());

    // callback 등록
    OnFrameBufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFrameBufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);

    // glfw 루프 실행, 윈도우 clsoe 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();          // imgui에 '지금부터 새로운 렌더링 프레임이다' 라고 명시적으로 알려줌
        ImGui::NewFrame();

        context->ProcessInput(window);
        context->Render();

        // opengl에서 그린 scene 위에 덮어서 imgui의 요소를 그릴 것이다
        // ImGui::Render()는 실제로 그림을 그리는 함수가 아니라 그림을 그릴 리스트를 종합하는 함수이다
        // ImGui::NewFrame() 호출부터 ImGui::Render() 호출 직전까지의 ImGui로 시작하는 UI 코드들의 구성요소 관한 데이터들을 모은다
        // 수집한 데이터들을 ImGui_ImplOpenGL3_RenderDrawData() 사용해 그린다
        // 그 뒤에 버퍼를 swap 한다
        ImGui::Render();        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());     
        glfwSwapBuffers(window);
    }
    
    context.reset();

    // 프로그램 종료 이후에 메모리를 정리하는 코드들
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwTerminate();

    return 0;
}