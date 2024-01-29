#include "shadow_map.h"


ShadowMapUPtr ShadowMap::Create(int width, int height) 
{
    auto shadowMap = ShadowMapUPtr(new ShadowMap());
    if (!shadowMap->Init(width, height))
    {
        return nullptr;
    }
    return std::move(shadowMap);
}

ShadowMap::~ShadowMap()
{
    if(m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void ShadowMap::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool ShadowMap::Init(int width, int height)
{
    glGenFramebuffers(1, &m_framebuffer);
    Bind();

    // GL_DEPTH_COMPONENT: depth map으로 이용하기 때문에
    m_shadowMap = Texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    m_shadowMap->SetFilter(GL_NEAREST, GL_NEAREST);
    m_shadowMap->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
    m_shadowMap->SetBorderColor(glm::vec4(1.0f));

    // depth attachment: frame buffer에 붙이려는 m_shadowMap 텍스처를 depth 에 붙인다
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowMap->Get(), 0);
    // color, stencil은 상관하지 않는다를 명시적으로 밝힘
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to complete shadow map framebuffer: {:x}", status);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return false;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}