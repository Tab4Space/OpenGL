#version 330 core

out float fragColor;            // 단일 채널 값 하나만 있으면 되기 때문에 float, color-attachment0 일 것이다
in vec2 texCoord;

// 간단한 구현을 위해 albedo는 생략
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform mat4 view;
uniform mat4 projection;
uniform vec2 noiseScale;        // noise 텍스처를 반복해서 붙일것이기 때문에 scale factor 사용
uniform float radius;           // 차폐를 어느정도 범위의 sample을 갖고와서 처리할지

const int KERNEL_SIZE = 64;         // 64개의 샘플 사용
const float BIAS = 0.025;           // depth 값 비교에 사용
uniform vec3 samples[KERNEL_SIZE];

void main() 
{
    // gPosition은 world coordinate을 기준으로 한 position
    // 이러한 position을 view 기반으로 바꿔야 한다
    vec4 worldPos = texture(gPosition, texCoord);
    if (worldPos.w <= 0.0f)
    {
        discard;
    }
    
    vec3 fragPos = (view * vec4(worldPos.xyz, 1.0)).xyz;                        // view space로 전환된 픽셀의 position
    vec3 normal = (view * vec4(texture(gNormal, texCoord).xyz, 0.0)).xyz;       // view space로 전환된 normal
    vec3 randomVec = texture(texNoise, texCoord * noiseScale).xyz;              // noise 텍스처로부터 얻어온 random vector
 
    // normal 방향과 수직하게 만들어주는 과정(Gram-Schmidt process)
    // 임의의 두 벡터로 orthogonal 하게 만든다
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 binormal = cross(normal, tangent);
    mat3 TBN = mat3(tangent, binormal, normal);

    // occlusion 계산
    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; i++) 
    {
        vec3 sample = fragPos + TBN * samples[i] * radius;
        vec4 screenSample = projection * vec4(sample, 1.0);
        screenSample.xyz /= screenSample.w;
        screenSample.xyz = screenSample.xyz * 0.5 + 0.5;
 
        float sampleDepth = (view * texture(gPosition, screenSample.xy)).z;
        // 반구보다 바깥쪽에 있는 샘플은 참여를 못하도록 막는다
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    fragColor = 1.0 - occlusion / KERNEL_SIZE;
}