#version 330 core

out float fragColor;            // 단일 채널 값 하나만 있으면 되기 때문에 float, color-attachment0 일 것이다
in vec2 texCoord;

// 간단한 구현을 위해 albedo는 생략
uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform mat4 view;

void main() 
{
    // gPosition은 world coordinate을 기준으로 한 position
    // 이러한 position을 view 기반으로 바꿔야 한다
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
    {
        discard;
    }
    
    // world -> view로 제대로 변환되는지 디버깅을 위한 코드
    fragColor = (view * vec4(worldPos.xyz, 1.0)).x * 0.1 + 0.5;
}