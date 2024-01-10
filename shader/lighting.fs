#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 viewPos;               // 바라보는 눈(카메라)의 위치

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;            // 표면이 얼마나 반짝거리는지 표현
};
uniform Material material;

void main()
{
    // 텍스처를 사용한 ambient
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

    // diffuse 계산
    vec3 lightDir = normalize(light.position - position);
    vec3 pixelNorm = normalize(normal);
    // diffuse factor * material의 diffuse * light의 diffuse
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * light.diffuse;

    // specular 계산
    vec3 specColor = texture2D(material.specular, texCoord).xyz;
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // specular factor
    // specular factor * material의 specular * light의 specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specColor * light.specular;

    // 최종 Color
    vec3 result = ambient + diffuse+ specular;
    fragColor = vec4(result, 1.0);
}