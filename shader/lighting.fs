#version 330 core

in vec3 normal;
in vec3 position;
in vec2 texCoord;
out vec4 fragColor;

uniform vec3 viewPos;               // 바라보는 눈(카메라)의 위치

struct Light
{
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform int blinn;

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
    float dist = length(light.position - position);     // 빛의 위치와 pixel의 3D위치 간의 거리
    vec3 distPoly = vec3(1.0, dist, dist*dist);         // 1, d, d*d

    // 내적
    float attenuation = 1.0 / dot(distPoly, light.attenuation);     // distPoly
    vec3 lightDir = (light.position - position)/dist;               

    // 두 벡터가 이루고 있는 내적 값(cos값) 계산
    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta-light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);

    // theta 이내의 light에 영향을 받는 것만 그린다
    if(intensity > 0.0)
    {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        float spec = 0.0;
        if(blinn == 0)
        {
            // phong shading
            vec3 viewDir = normalize(viewPos - position);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else
        {
            // blinn-phong shading
            vec3 viewDir = normalize(viewPos - position);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        vec3 specular = spec * specColor * light.specular;
        result += (diffuse + specular)*intensity;
    }

    result *= attenuation;
    
    fragColor = vec4(result, 1.0);
    // fragColor = vec4(vec3(gl_FragCoord.z), 1.0f);        // depth visualize
}