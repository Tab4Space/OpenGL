#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "shadow_map.h"


CLASS_PTR(Context)
class Context {
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

    void DrawScene(const glm::mat4& view, const glm::mat4& projection, const Program* program);

private:
    Context() {}
    bool Init();
  
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_pbrProgram;

    MeshUPtr m_box;
    MeshUPtr m_plane;
    MeshUPtr m_sphere;

    TextureUPtr m_hdrMap;
    ProgramUPtr m_sphericalMapProgram;
    CubeTexturePtr m_hdrCubeMap;

    struct Light 
    {
        glm::vec3 position { glm::vec3(0.0f, 0.0f, 0.0f) };
        glm::vec3 color { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    std::vector<Light> m_lights;
 
    struct Material 
    {
        glm::vec3 albedo { glm::vec3(1.0f, 1.0f, 1.0f) };
        float roughness { 0.5f };
        float metallic { 0.5f };
        float ao { 0.1f };
    };
    Material m_material;

    // for texture pbr
    // struct Material 
    // {
    //     TexturePtr albedo;
    //     TexturePtr roughness;
    //     TexturePtr metallic;
    //     TexturePtr normal;
    //     float ao { 0.1f };
    // };
    // Material m_material;

    // screen size
    int m_width {WINDOW_WIDTH};
    int m_height {WINDOW_HEIGHT};

    // camera parameter
    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };
    float m_cameraPitch { 0.0f };
    float m_cameraYaw { 0.0f };
    glm::vec3 m_cameraFront { glm::vec3(0.0f, -1.0f, 0.0f) };
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 8.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
};

#endif