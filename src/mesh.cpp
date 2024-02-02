#include "mesh.h"


void Material::SetToProgram(const Program* program) const
{
    int textureCount = 0;
    if(diffuse)
    {
        // diffuse가 있으면 0번 슬롯부터 사용해서 diffuse 세팅
        glActiveTexture(GL_TEXTURE0+textureCount);
        program->SetUniform("material.diffuse", textureCount);
        diffuse->Bind();
        textureCount++;
    }

    if(specular)
    {
        // specular가 있으면 0번+textureCount 슬롯부터 사용해서 diffuse 세팅
        glActiveTexture(GL_TEXTURE0+textureCount);
        program->SetUniform("material.specular", textureCount);
        specular->Bind();
        textureCount++;
    }

    glActiveTexture(GL_TEXTURE0);
    program->SetUniform("material.shininess", shininess);
}

MeshUPtr Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType)
{
    auto mesh = MeshUPtr(new Mesh());
    mesh->Init(vertices, indices, primitiveType);
    return std::move(mesh);
}

MeshUPtr Mesh::CreateBox()
{
    //Vertex(위치, normal 방향, texture coordinate)
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },

        Vertex { glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::CreatePlane()
{
    std::vector<Vertex> vertices = {
        Vertex { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
        Vertex { glm::vec3( 0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
        Vertex { glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec3( 0.0f,  0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
    };

    std::vector<uint32_t> indices = {
        0,  1,  2,  2,  3,  0,
    };

    return Create(vertices, indices, GL_TRIANGLES);
}

MeshUPtr Mesh::CreateSphere(uint32_t latiSegmentCount, uint32_t longiSegmentCount) 
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t circleVertCount = longiSegmentCount + 1;
    vertices.resize((latiSegmentCount + 1) * circleVertCount);
    
    for (uint32_t i = 0; i <= latiSegmentCount; i++) 
    {
        float v = (float)i / (float)latiSegmentCount;
        float phi = (v - 0.5f) * glm::pi<float>();
        auto cosPhi = cosf(phi);
        auto sinPhi = sinf(phi);
        
        for (uint32_t j = 0; j <= longiSegmentCount; j++) 
        {
            float u = (float)j / (float)longiSegmentCount;
            float theta = u * glm::pi<float>() * 2.0f;
            
            auto cosTheta = cosf(theta);
            auto sinTheta = sinf(theta);
            auto point = glm::vec3(cosPhi * cosTheta, sinPhi, -cosPhi * sinTheta);
      
            vertices[i * circleVertCount + j] = Vertex {point * 0.5f, point, glm::vec2(u, v), glm::vec3(0.0f)};
        }
    }

    indices.resize(latiSegmentCount * longiSegmentCount * 6);
    for (uint32_t i = 0; i < latiSegmentCount; i++) 
    {
        for (uint32_t j = 0; j < longiSegmentCount; j++) 
        {
            uint32_t vertexOffset = i * circleVertCount + j;
            uint32_t indexOffset = (i * longiSegmentCount + j) * 6;

            indices[indexOffset    ] = vertexOffset;
            indices[indexOffset + 1] = vertexOffset + 1;
            indices[indexOffset + 2] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 3] = vertexOffset;
            indices[indexOffset + 4] = vertexOffset + 1 + circleVertCount;
            indices[indexOffset + 5] = vertexOffset + circleVertCount;
        }
    }

    return Create(vertices, indices, GL_TRIANGLES);
}

void Mesh::Draw(const Program* program) const
{
    // 실제로 그림을 그리는 코드
    m_vertexLayout->Bind();         // VAO 바인딩
    if(m_material)
    {
        m_material->SetToProgram(program);
    }
    glDrawElements(m_primitiveType, m_indexBuffer->GetCount(), GL_UNSIGNED_INT, 0);     // 
}

void Mesh::ComputeTangents(std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
{
    // 하나의 삼각형을 이루고 있는 position과 uv를 전달해서 그에 대한 postion1을 기준으로 한 tangent vector를 리턴
    auto compute = [](const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3, 
        const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)->glm::vec3
    {
        auto edge1 = pos2 - pos1;
        auto edge2 = pos3 - pos1;

        auto deltaUV1 = uv2 - uv1;
        auto deltaUV2 = uv3 - uv1;

        // 역행렬 구할 때 (ad - bc)부분, 결과가 0이면 역행렬이 없다는 뜻이다
        float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

        if(det != 0.0f)
        {
            auto invDet = 1.0f / det;
            return invDet * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        }
        else
        {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
    };

    // initialize
    std::vector<glm::vec3> tangents;
    tangents.resize(vertices.size());
    memset(tangents.data(), 0, tangents.size()*sizeof(glm::vec3));

    // 모든 vertex에 대해서 tangent 계산
    for(size_t i=0; i<indices.size(); i+=3)
    {
        // 각 vertex의 인덱스
        auto v1 = indices[i];
        auto v2 = indices[i+1];
        auto v3 = indices[i+2];

        // v1의 tangent
        tangents[v1] += compute(
            vertices[v1].position, vertices[v2].position, vertices[v3].position,
            vertices[v1].texCoord, vertices[v2].texCoord, vertices[v3].texCoord
        );

        // v2의 tangent
        tangents[v2] = compute(
            vertices[v2].position, vertices[v3].position, vertices[v1].position,
            vertices[v2].texCoord, vertices[v3].texCoord, vertices[v1].texCoord
        );

        // v3의 tangent
        tangents[v3] = compute(
            vertices[v3].position, vertices[v1].position, vertices[v2].position,
            vertices[v3].texCoord, vertices[v1].texCoord, vertices[v2].texCoord
        );

        // normalize
        for(size_t i=0; i<vertices.size(); i++)
        {
            vertices[i].tangent = glm::normalize(tangents[i]);
        }
    }
}

void Mesh::Init(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices, uint32_t primitiveType)
{
    /*
    * vertex layout 초기화 함수
    */
   
    if(primitiveType == GL_TRIANGLES)
    {
        // const_cast
        // Init의 vertices는 const를 사용하는데 ComputeTangents는 const를 사용 안함
        // 안 붙어있는 쪽의 파라미터를 붙어있는 쪽의 파라미터로 사용하는 것은 가능, 반대는 불가능
        // 따라서 형변환을 해서 넣어야 하며, 이 때 const_cast를 사용(const를 붙이거나 떼어내거나, 보통은 떼어내서 사용)
        // 아래에서는 const_cast를 사용해 이 부분에서만 명시적으로 const를 떼고 사용하겠다는 의미
        ComputeTangents(const_cast<std::vector<Vertex>&>(vertices), indices);
    }

    m_vertexLayout = VertexLayout::Create();
    
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices.data(), sizeof(Vertex), indices.size());
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices.data(), sizeof(uint32_t), indices.size());

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);                                // position
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, normal));         // normal
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, texCoord));       // texture coordination
    m_vertexLayout->SetAttrib(3, 3, GL_FLOAT, false, sizeof(Vertex), offsetof(Vertex, tangent));        // tangent
}