#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__

#include "texture.h"


CLASS_PTR(FrameBuffer);
class FrameBuffer
{
public:
    static FrameBufferUPtr Create(const TexturePtr colorAttachment);
    static void BindToDefault();
    ~FrameBuffer();

    const uint32_t Get() const { return m_framebuffer; }
    void Bind() const;
    const TexturePtr GetColorAttachment() const { return m_colorAttachment; }

private:
    FrameBuffer() {}
    bool InitWithColorAttachment(const TexturePtr colorAttachment);

    uint32_t m_framebuffer { 0 };
    uint32_t m_depthStencilBuffer { 0 };        // render buffer로 만들 것
    TexturePtr m_colorAttachment;               // color buffer, 이것은 texture로 만들어서 붙일 것
};

#endif