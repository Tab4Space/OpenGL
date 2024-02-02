#version 330 core

out vec4 fragColor;
in vec2 texCoord;
uniform sampler2D tex;

void main() 
{
    /* 
    텍스처 좌표값이 0~1로 normalize 되어있는데, 그 값에 texelsize를 더하고 빼는 식으로
    주변에 있는 픽셀들의 위치를 계산
    */

    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    vec4 result = vec4(0.0);

    // -2 ~ 2 범위에 있는 픽셀 조사
    for (int x = -2; x <= 2; ++x) 
    {
        for (int y = -2; y <= 2; ++y) 
        {
            // 하나의 픽셀 간격이 offset으로 나온다
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(tex, texCoord + offset);
        }
    }
    fragColor = result / 25.0;
}