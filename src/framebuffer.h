#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"


CLASS_PTR(Framebuffer);
class Framebuffer
{
public:
    static FramebufferUPtr Create(const std::vector<TexturePtr>& colorAttachments);
    static void BindToDefault();
    ~Framebuffer();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    int GetColorAttachmentCount() const { return (int)m_colorAttachments.size(); }
    const TexturePtr GetColorAttachment(int index = 0) const { return m_colorAttachments[index]; }

private:
    Framebuffer() {}
    bool InitWithColorAttachments(const std::vector<TexturePtr>& colorAttachments);

    uint32_t m_framebuffer { 0 };
    uint32_t m_depthStencilBuffer { 0 };        // render buffer로 만들 것
    std::vector<TexturePtr> m_colorAttachments; // 여러개의 color attachments를 받는다

    //TexturePtr m_colorAttachment;               // color buffer, 이것은 texture로 만들어서 붙일 것
    
};

#endif