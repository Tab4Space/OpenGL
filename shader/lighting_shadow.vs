#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VS_OUT
{
    vec3 fragPos;               // fragment postion, modelTransform이 적용된 것
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;          // 빛을 기준으로 pixel(fragment) postion 담당
} vs_out;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;    // 빛은 기준으로 model, view, projection을 다 거쳤을 때의 위치

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    vs_out.fragPos = vec3(modelTransform * vec4(aPos, 1.0));                // 월드 좌표를 기준으로 한다
    vs_out.normal = transpose(inverse(mat3(modelTransform))) * aNormal;
    vs_out.texCoord = aTexCoord;
    vs_out.fragPosLight = lightTransform * vec4(vs_out.fragPos, 1.0);       // light를 기준으로 한 view, projeciton을 곱한게 들어간다
}