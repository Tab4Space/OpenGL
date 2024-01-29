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
    int directional;
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

float ShadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    // projection matrix를 거치면 w에 1이 아닌 다른 값이 있다 > w값으로 나누면 진짜 값이 나온다 (modeling 첫번째 강의 참고)
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // transform to [0,1] range
    // * 0.5 + 0.5 이유: projCoords 값의 범위가 canonical coord를 기준으로 transpose진행 
    // canonical은 -1 ~ +1 범위 > 계산을 거쳐서 0 ~ 1 사이로 맞춰준다
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light’s perspective (using
    // [0,1] range fragPosLight as coords)
    // shadow map으로부터 xy를 가지고 좌표 r값(첫 번재 채널)을 가져온다
    // 빛의 위치에서 가장 가까이 있는 depth 위치이다
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from light’s perspective
    // 이 위치의 z좌표가 이 픽셀의 depth 값이다
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    // 비교를 통해 shadow 값 얻음
    // normal과 lightDir 방향이 같으면 1이 나올 것이고 여기에서 1을 빼면 0에 가까워진다
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001);
    
    // PCF
    float shadow = 0.0;
    // 이웃하는 픽셀(9개)에 접근하기 위해
    // 샘플을 많이 사용할 수록 더 부드러워진다 > 너무 샘플링이 많다면 성능 저하발생
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) 
    {
        for (int y = -1; y <= 1; ++y) 
        {
            float pcfDepth = texture(shadowMap,
            projCoords.xy + vec2(x, y) * texelSize).r;
            // 누적하면 최대 9 ~ 최소 0
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

void main()
{
    vec3 texColor = texture2D(material.diffuse, fs_in.texCoord).xyz;
    vec3 ambient = texColor * light.ambient;

    vec3 result = ambient;
    vec3 lightDir;
    float intensity = 1.0;
    float attenuation = 1.0;
    if (light.directional == 1) 
    {
        // direction light 사용하니까 direction 값만 사용
        lightDir = normalize(-light.direction);
    }
    else 
    {
        // direction light가 아니면 attenation, intensity 등 다른 것들 계산
        float dist = length(light.position - fs_in.fragPos);
        vec3 distPoly = vec3(1.0, dist, dist*dist);
        attenuation = 1.0 / dot(distPoly, light.attenuation);
        lightDir = (light.position - fs_in.fragPos) / dist;
 
        float theta = dot(lightDir, normalize(-light.direction));
        intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0, 1.0);
    }

    if(intensity > 0.0)
    {
        vec3 pixelNorm = normalize(fs_in.normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, fs_in.texCoord).xyz;
        float spec = 0.0;
        if (blinn == 0) 
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
        float shadow = ShadowCalculation(fs_in.fragPosLight, pixelNorm, lightDir);

        //shadow 함수 리턴에 따라서 result 가 0 or 1이 된다
        result += (diffuse - specular) * intensity * (1.0 - shadow);
    }
    
    result *= attenuation;
    fragColor = vec4(result, 1.0);
}
