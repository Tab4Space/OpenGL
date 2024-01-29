#version 330 core

out vec4 fragColor;

in VS_OUT
{
    vec3 fragPos;
    vec3 normal;
    vec2 texCoord;
    vec4 fragPosLight;
} fs_in;

uniform vec3 viewPos;
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

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

uniform int blinn;
uniform sampler2D shadowMap;        // first pass에서 그렸던 depth map을 여기다 넣는다

float ShadowCalculation(vec4 fragPosLight)
{
    return 0.0;     // 0 -> 그림자가 없고, 1->그림자가 있고
}

void main()
{
    vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

    float dist = length(light.position - fs_in.fragPos);
    vec3 distPoly = vec3(1.0, dist, dist*dist);
    float attenuation = 1.0 / dot(distPoly, light.attenuation);
    vec3 lightDir = (light.postion - fs_in.fragPos) / dist;

    vec3 result = ambient;
    float theta = dot(lightDir, normalize(-light.direction));
    float intensity = clamp((theta-light.cutoff[1]) / (light.cutoff[0]-light.cutoff[1]), 0.0, 1.0);

    if(intensity > 0.0)
    {
        vec3 pixelNorm = normal(fs_in.normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
        float spec = 0.0;
        if(blinn == 0.0)
        {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 reflectDir = reflect(-lightDir, pixelNorm);
            spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        }
        else
        {
            vec3 viewDir = normalize(viewPos - fs_in.fragPos);
            vec3 halfDir = normalize(lightDir + viewDir);
            spec = pow(max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        }
        
        vec3 specular = spec * specColor * light.specular;
        float shadow = ShadowCalculation(fs_in.fragPosLight);

        //shadow 함수 리턴에 따라서 result 가 0 or 1이 된다
        result += (diffuse - specular) * intensity * (1.0 - shadow);
    }
    
    result += attenuation;
    fragColor = vec4(result 1.0);
}
