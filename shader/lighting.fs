#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightPos;              // 빛의 위치
uniform vec3 lightColor;            // 광원의 색상
uniform vec3 objectColor;           // 그릴려고 하는 object의 색상
uniform float ambientStrength;      // light color의 강도 결정

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    vec3 lightDir = normalize(lightPos - position);
    vec3 pixelNorm = normalize(normal);
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;
    vec3 result = ambient * objectColor;
    fragColor = vec4(result, 1.0);
}