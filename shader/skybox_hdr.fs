#version 330 core

out vec4 fragColor;
in vec3 localPos;

uniform samplerCube cubeMap;
uniform float roughness;

void main() 
{
    vec3 envColor = textureLod(cubeMap, localPos, roughness * 4).rgb;
    envColor = envColor / (envColor + vec3(1.0));   // reinhard tone mapping
    envColor = pow(envColor, vec3(1.0/2.2));        // sRGB로 바꾸기
    fragColor = vec4(envColor, 1.0);
}