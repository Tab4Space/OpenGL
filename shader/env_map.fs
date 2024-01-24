#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    vec3 I = normalize(position - cameraPos);       // 눈으로 바라보는 벡터 (vertex의 position - camera position)
    vec3 R = reflect(I, normalize(normal));         // 위의 결과와 normal vector 간의 reflection vector
    fragColor = vec4(texture(skybox, R).rgb, 1.0);  // reflection vector 방향으로 ray를 쏴서 큐브맵 픽셀의 rgb를 가져와서 그림을 그린다
}