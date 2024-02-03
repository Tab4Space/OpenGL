#include "framebuffer.h"


FramebufferUPtr Framebuffer::Create(const std::vector<TexturePtr>& colorAttachments)
{
    auto framebuffer = FramebufferUPtr(new Framebuffer());
    if(!framebuffer->InitWithColorAttachments(colorAttachments))
    {
        return nullptr;
    }
    return std::move(framebuffer);
}

Framebuffer::~Framebuffer()
{
    if(m_depthStencilBuffer)
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if(m_framebuffer)
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void Framebuffer::BindToDefault()
{
    // 화면에 그림을 그리는 형태로 세팅
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind() const
{
    // frame buffer를 만들고, buffer에 그림을 그리고 싶으면 bind를 한다
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

bool Framebuffer::InitWithColorAttachments(const std::vector<TexturePtr>& colorAttachments)
{
    /* deferred shading */
    m_colorAttachments = colorAttachments;                // color buffer로 이용할 텍스처 세팅
    glGenFramebuffers(1, &m_framebuffer);               // frame buffer object 생성
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);   // 생성한거 바인드
                                                        // 여기까지하면 기본화면에 그리는 것이 아니라 새로운 frame buffer에 그림을 그린다
    for(size_t i=0; i<m_colorAttachments.size(); i++)
    {
        // 각 color attachment마다 각 텍스처를 붙임
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, m_colorAttachments[i]->Get(), 0);
    }

    if(m_colorAttachments.size() > 0)
    {
        std::vector<GLenum> attachments;
        attachments.resize(m_colorAttachments.size());
        for(size_t i=0; i<m_colorAttachments.size(); i++)
        {
            attachments[i] = GL_COLOR_ATTACHMENT0+i;
        }
        // 여러 개의 color attachment 를 사용할 경우 명시적으로 알려야 한다
        glDrawBuffers(m_colorAttachments.size(), attachments.data());
    }

    int width = m_colorAttachments[0]->GetWidth();
    int height = m_colorAttachments[0]->GetHeight();

    // depth/stencil 버퍼 붙이기
    glGenRenderbuffers(1, &m_depthStencilBuffer);               // depth&stencil은 렌더 버퍼로 만들어서 붙일 것
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);  // 바인드
    // 렌더 버퍼의 사이즈, 픽셀 포맷등을 설정
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    // 디폴트 렌더버퍼로 다시 바인딩
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

   // 바인딩했던 frame buffer(m_framebuffer)에 depth&stencil을 attach, render buffer attach
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);
    // frame buffer에 사용될 render 버퍼와 texture를 끼워주는 코드 end

    // frame buffer의 상태 확인
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(result != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();
    return true;

    /* forward shading 방법
    m_colorAttachments = colorAttachments;                // color buffer로 이용할 텍스처 세팅
    glGenFramebuffers(1, &m_framebuffer);               // frame buffer object 생성
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);   // 생성한거 바인드
                                                        // 여기까지하면 기본화면에 그리는 것이 아니라 새로운 frame buffer에 그림을 그린다

    // frame buffer에 사용될 render 버퍼와 texture를 끼워주는 코드
    // frame buffer에 텍스처 붙이기, color/stencil/depth buffer에 어떤 텍스처를 사용하고 싶을 때 사용하는 함수
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment->Get(), 0);

    // depth/stencil 버퍼 붙이기
    glGenRenderbuffers(1, &m_depthStencilBuffer);               // depth&stencil은 렌더 버퍼로 만들어서 붙일 것
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);  // 바인드
    // 렌더 버퍼의 사이즈, 픽셀 포맷등을 설정
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, colorAttachment->GetWidth(), colorAttachment->GetHeight());
    // 디폴트 렌더버퍼로 다시 바인딩
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // 바인딩했던 frame buffer(m_framebuffer)에 depth&stencil을 attach, render buffer attach
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);
    // frame buffer에 사용될 render 버퍼와 texture를 끼워주는 코드 end

    // frame buffer의 상태 확인
    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(result != GL_FRAMEBUFFER_COMPLETE)
    {
        SPDLOG_ERROR("failed to create framebuffer: {}", result);
        return false;
    }

    BindToDefault();
    return true;
    // forward shading 방법 end*/
}


CubeFramebufferUPtr CubeFramebuffer::Create(const CubeTexturePtr colorAttachment) 
{
    auto framebuffer = CubeFramebufferUPtr(new CubeFramebuffer());
    if (!framebuffer->InitWithColorAttachment(colorAttachment))
    {
        return nullptr;
    }
    return std::move(framebuffer);
}

CubeFramebuffer::~CubeFramebuffer() 
{
    if (m_depthStencilBuffer) 
    {
        glDeleteRenderbuffers(1, &m_depthStencilBuffer);
    }
    if (m_framebuffer) 
    {
        glDeleteFramebuffers(1, &m_framebuffer);
    }
}

void CubeFramebuffer::Bind(int cubeIndex) const 
{
    // 큐브의 몇 번째 인덱스(positive X, negative X 등등) 
    // color attachment가 cube texture 하나로 되어있기 때문에 그것을 framebuffer 로 생성
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + cubeIndex, m_colorAttachment->Get(), 0);
}

bool CubeFramebuffer::InitWithColorAttachment(const CubeTexturePtr& colorAttachment) 
{
    m_colorAttachment = colorAttachment;
    glGenFramebuffers(1, &m_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X, m_colorAttachment->Get(), 0);

    int width = m_colorAttachment->GetWidth();
    int height = m_colorAttachment->GetHeight();

    glGenRenderbuffers(1, &m_depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result != GL_FRAMEBUFFER_COMPLETE) 
    {
        SPDLOG_ERROR("failed to create framebuffer: 0x{:04x}", result);
        return false;
    }

    Framebuffer::BindToDefault();

    return true;
}