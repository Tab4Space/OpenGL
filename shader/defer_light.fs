#version 330 core

out vec4 fragColor;
in vec2 texCoord;

// first pass에서 만든 3개의 텍스처를 받는다
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
// ssao map, 사용유무
uniform sampler2D ssao;
uniform int useSsao;

// point 라이트를 사용할건데, attenuation은 생략
struct Light {
    vec3 position;
    vec3 color;
};

const int NR_LIGHTS = 32;           // 32개의 light 사용
uniform Light lights[NR_LIGHTS];    // light 배열
uniform vec3 viewPos;               // specular 계산에 필요

void main() 
{
    // retrieve data from G-buffer
    vec3 fragPos = texture(gPosition, texCoord).rgb;
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 albedo = texture(gAlbedoSpec, texCoord).rgb;
    float specular = texture(gAlbedoSpec, texCoord).a;
    
    // then calculate lighting as usual
    vec3 ambient = useSsao == 1 ? texture(ssao, texCoord).r * 0.4 * albedo : albedo * 0.4;      // 0.4는 하드코딩 값
    vec3 lighting = ambient;
    vec3 viewDir = normalize(viewPos - fragPos);

    for(int i = 0; i < NR_LIGHTS; ++i) 
    {
        // diffuse
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * lights[i].color;
        lighting += diffuse;
    }
    fragColor = vec4(lighting, 1.0);
}