#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "program.h"

struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
};

CLASS_PTR(Material)
class Material
{
public:
    static MaterialUPtr Create() { return MaterialUPtr(new Material()); }
    TexturePtr diffuse;
    TexturePtr specular;
    float shininess { 32.0f };

    // 여기서 program은 lighting.vs & lighting.fs로 만들어진 program shader를 사용
    void SetToProgram(const Program* program) const;

private:
    Material() {}
};

CLASS_PTR(Mesh)
class Mesh
{
public:
    static MeshUPtr Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType);
    static MeshUPtr CreateBox();
    static MeshUPtr CreatePlane();
    static MeshUPtr CreateSphere(uint32_t latiSegmentCount = 16, uint32_t longiSegmentCount = 32);

    const VertexLayout* GetVertexLayout() const { return m_vertexLayout.get(); }
    BufferPtr GetVertexBuffer() const { return m_vertexBuffer; }
    BufferPtr GetIndexBuffer() const { return m_indexBuffer; }

    void SetMaterial(MaterialPtr material) { m_material = material; }
    MaterialPtr GetMaterial() const { return m_material; }

    void Draw(const Program* program) const;

    // 인자는 create() 와 같은 의미
    static void ComputeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

private:
    Mesh() {};
    void Init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType);

    uint32_t m_primitiveType { GL_TRIANGLES };      // 그림을 그릴 타입(삼각형, 라인, 사각형 등등)
    VertexLayoutUPtr m_vertexLayout;
    BufferPtr m_vertexBuffer;
    BufferPtr m_indexBuffer;

    MaterialPtr m_material;
};

#endif