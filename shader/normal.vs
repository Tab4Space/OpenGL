#version 330 core

// normal 방향에 대한 attribute는 일부로 넣지 않음 (나중에 넣는다)
// texture로부터 normal vector를 얻어서 사용하기 때문에 당장에 필요하진 않음
// 원래는 locaiton = 1 이 normal vector이다
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

uniform mat4 transform;
uniform mat4 modelTransform;

// 월드 좌표상의 각각의 버텍스의 normal vector가 들어가지만 이 쉐이더 코드에서는 빠짐
out vec2 texCoord;
out vec3 position;
// normal, tangent 는 modelTransform의 역행렬을 구해서 곱하고 world coordinate을 구한다
out vec3 normal;
out vec3 tangent;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    texCoord = aTexCoord;
    position = (modelTransform * vec4(aPos, 1.0)).xyz;

    mat4 invTransModelTransform = transpose(inverse(modelTransform));
    normal = (invTransModelTransform * vec4(aNormal, 0.0)).xyz;
    tangent = (invTransModelTransform * vec4(aTangent, 0.0)).xyz;
}