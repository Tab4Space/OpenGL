#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 lightPos;              // 빛의 위치
uniform vec3 lightColor;            // 광원의 색상
uniform vec3 objectColor;           // 그릴려고 하는 object의 색상
uniform float ambientStrength;      // light color의 강도 결정

uniform float specularStrength;
uniform float specularShininess;
uniform vec3 viewPos;               // 바라보는 눈(카메라)의 위치

void main()
{
    // ambient 계산
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 계산
    vec3 lightDir = normalize(lightPos - position);
    vec3 pixelNorm = normalize(normal);
    vec3 diffuse = max(dot(pixelNorm, lightDir), 0.0) * lightColor;

    // specular 계산
    vec3 viewDir = normalize(viewPos - position);
    // lightDir 방향으로 입사한 빛이 다시 반사되어 나오는 방향벡터를 구함
    // -lightDir이 입사한 방향을 의미
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // 지수함수를 쓰는 이유는 
    // - max 결과가 0~1 사이인데, specularShininess가 커질수록 값이 작아진다
    // - specular light가 만드는 영역이 생기는데, 점점 작아지는 효과를 만든다
    // - specular light의 밝기가 아닌 만드는 영역을 제어하는 목적
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);
    vec3 specular = specularStrength * spec * lightColor;

    // 최종 결과
    vec3 result = (ambient + diffuse+ specular) * objectColor;
    fragColor = vec4(result, 1.0);
}