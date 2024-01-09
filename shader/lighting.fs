#version 330 core
in vec3 normal;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightColor;            // 광원의 색상
uniform vec3 objectColor;           // 그릴려고 하는 object의 색상
uniform float ambientStrength;      // light color의 강도 결정

void main()
{
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;
    fragColor = vec4(result, 1.0);
}