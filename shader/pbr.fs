#version 330 core
in vec3 normal;
in vec2 texCoord;
in vec3 fragPos;

out vec4 fragColor;

uniform vec3 viewPos;

// light는 4개 사용
struct Light 
{
    vec3 position;
    vec3 color;
};
const int LIGHT_COUNT = 4;
uniform Light lights[LIGHT_COUNT];

struct Material 
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};
uniform Material material;

uniform samplerCube irradianceMap;
uniform samplerCube preFilteredMap;
uniform sampler2D brdfLookupTable;
uniform int useIBL;

const float PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 halfDir, float roughness) 
{
    /*
    normal 방항의 distribution function
    */
    float a = roughness * roughness;
    float a2 = a * a;
    float dotNH = max(dot(normal, halfDir), 0.0);
    float dotNH2 = dotNH * dotNH;

    float num = a2;
    float denom = (dotNH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float dotNV, float roughness) 
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;  
    
    float num = dotNV;
    float denom = dotNV * (1.0 - k) + k;
    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness) 
{
    /*
    geometry function
    */

    float dotNV = max(dot(normal, viewDir), 0.0);
    float dotNL = max(dot(normal, lightDir), 0.0);
    float ggx2 = GeometrySchlickGGX(dotNV, roughness);
    float ggx1 = GeometrySchlickGGX(dotNL, roughness);
    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) 
{
    /*
    frenel equation의 approximation
    */
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) 
{
    // specular term을 알아낸다
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() 
{
    // 재질 파라미터
    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;

    // vertex shader에서 input으로 들어오는 값
    vec3 fragNormal = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);
    float dotNV = max(dot(fragNormal, viewDir), 0.0);

    // Frenel equation
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 outRadiance = vec3(0.0);
    for (int i = 0; i < LIGHT_COUNT; i++) 
    {
        // calculate per-light radiance
        vec3 lightDir = normalize(lights[i].position - fragPos);
        vec3 halfDir = normalize(viewDir + lightDir);

        float dist = length(lights[i].position - fragPos);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance = lights[i].color * attenuation;

        // Cook-Torrance BRDF
        float ndf = DistributionGGX(fragNormal, halfDir, roughness);
        float geometry = GeometrySmith(fragNormal, viewDir, lightDir, roughness);
        vec3 fresnel = FresnelSchlick(max(dot(halfDir, viewDir), 0.0), F0);

        vec3 kS = fresnel;      // kS는 frenel 값으로 대체 가능
        vec3 kD = 1.0 - kS;
        kD *= (1.0 - metallic);

        float dotNL = max(dot(fragNormal, lightDir), 0.0);
        vec3 numerator = ndf * geometry * fresnel;              // 수식에서 분자 부분
        float denominator = 4.0 * dotNV * dotNL;                // 수식에서 분모 부분
        vec3 specular = numerator / max(denominator, 0.001);    // specular term

        // add to outgoing radiance Lo
        outRadiance += (kD * albedo / PI + specular) * radiance * dotNL;
    }

    /*
    여기서 light color는 0~1 사이가 아니고 밝기 값이 들어가기 때문에 HDR 기준으로 사용한다
    따라서 tone mapping 필요
    */
    vec3 ambient = vec3(0.03) * albedo * ao;
    if (useIBL == 1) 
    {
        vec3 kS = FresnelSchlickRoughness(dotNV, F0, roughness);
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        vec3 irradiance = texture(irradianceMap, fragNormal).rgb;
        vec3 diffuse = irradiance * albedo;

        // 기존에 없던 reflect-term 계싼
        vec3 R = reflect(-viewDir, fragNormal);
        const float MAX_REFLECTION_LOD = 4.0;
        
        // prefiltered, brdf-lookup 반영해서 specular 계산
        vec3 preFilteredColor = textureLod(preFilteredMap, R, roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBrdf = texture(brdfLookupTable, vec2(dotNV, roughness)).rg;
        vec3 specular = preFilteredColor * (kS * envBrdf.x + envBrdf.y);

        ambient = (kD * diffuse + specular) * ao;
    }
    vec3 color = ambient + outRadiance;

    // Reinhard tone mapping + gamma correction
    color = color / (color + 1.0);
    color = pow(color, vec3(1.0 / 2.2));            // linear RGB를 sRGB로 바꿔준다

    fragColor = vec4(color, 1.0);
}