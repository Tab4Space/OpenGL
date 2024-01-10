#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;             // model mat * view mat * projection mat
uniform mat4 modelTransform;        // 로컬 좌표를 월드 좌표로 바꾸는 model matrix

out vec3 normal;
out vec2 texCoord;
out vec3 position;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    normal = (transpose(inverse(modelTransform))*vec4(aNormal, 0.0)).xyz;
    texCoord = aTexCoord;
    position = (modelTransform*vec4(aPos, 1.0)).xyz;        // vertex의 position을 따로 계산 > 월드 좌표계 상의 각 정점의 위치
}