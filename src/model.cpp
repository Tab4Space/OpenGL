#include "model.h"

ModelUPtr Model::Load(const std::string& filename)
{
    auto model = ModelUPtr(new Model());
    if(!model->LoadByAssimp(filename))
    {
        return nullptr;
    }
    return std::move(model);
}

void Model::Draw()
{
    for(auto& mesh : m_meshes)
    {
        mesh->Draw();
    }
}

bool Model::LoadByAssimp(const std::string& filename)
{
    Assimp::Impoter importer;
    auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCIMPLETE || !scene->mRootNode)
    {
        SPDLOG_ERROR("failed to load model: {}", filename);
        return false;
    }
    ProcessNode(scene->mRootNode, scene);
    return true;
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    SPDLOG_INFO("process mesh: {}, #vert: {}, #face: {}", mesh->mName.C_Str(), mesh->mNumVertices, mesh->mNumFaces);

    std::vector<Vertex> vertices;
    vertices.resize(mesh->mNumVertices);            // vertex 개수

    // vertex array 만들기
    for(uint32_t i=0; i<mesh->mNumVertices; i++)
    {
        auto& v = vertices[i];
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);   // vertex 세팅
        v.normal = glm::vec3(mesh->mNormals[i].x, mNormals[i].y, mNormals[i].z);                    // normal 세팅
        v.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);         // texture 세팅
    }

    // index array 만들기
    std::vector<uint32_t> indices;
    indices.resize(mesh->mNumFaces * 3);            // triangle 개수 * 3 = index 개수
    for(uint32_t i=0; i<mesh->mNumFaces; i++)
    {
        indices[3*i] = mesh->mFaces[i].mIndices[0];
        indices[3*i+1] = mesh->mFaces[i].mIndices[1];
        indices[3*i+2] = mesh->mFaces[i].mIndices[2];
    }

    auto glMesh = Mesh::Create(vertices, indices, GL_TRIANGLES);
    m_meshes.push_back(std::move(glMesh));
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
    for(uint32_t i=0; i<node->mNumMeshes; i++)
    {
        auto meshIndex = node->mMeshes[i];
        auto mesh = scene->mMeshes[meshIndex];
        ProcessMesh(mesh, scene);
    }

    for(uint32_t i=0; i<node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}