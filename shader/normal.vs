#version 330 core

// normal 방향에 대한 attribute는 일부로 넣지 않음 (나중에 넣는다)
// texture로부터 normal vector를 얻어서 사용하기 때문에 당장에 필요하진 않음
// 원래는 locaiton = 1 이 normal vector이다
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 transform;
uniform mat4 modelTransform;

// 월드 좌표상의 각각의 버텍스의 normal vector가 들어가지만 이 쉐이더 코드에서는 빠짐
out vec2 texCoord;
out vec3 position;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;
}