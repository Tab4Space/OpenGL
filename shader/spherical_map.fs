#version 330 core

out vec4 fragColor;
in vec3 localPos;
uniform sampler2D tex;

const vec2 invPi = vec2(0.1591549, 0.3183098862);

vec2 SampleSphericalMap(vec3 v) 
{
    // arctan, arcsin을 이용해 uv 값을 역으로 계산한다
    // mesh.cpp에서 구체를 만드는 함수가 있는데, 그것의 반대이다
    return vec2(atan(v.z, v.x), asin(v.y)) * invPi + 0.5;
}

void main() 
{
    vec2 uv = SampleSphericalMap(normalize(localPos)); // normalize된 어떤 방향 벡터
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}