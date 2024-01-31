#version 330 core

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 position;
in vec3 normal;
in vec3 texCoord;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
};
uniform Material material;

void main()
{
    gPosition = vec4(position, 1.0);
    gNormal = vec4(normalize(normal), 1.0);
    gAlbedoSpec.rgb = texture(material.diffuse, texCoord).rgb;      // diffuse color 텍스처를 가져와서 color를 채움
    gAlbedoSpec.a = texture(material.specular, texture).r;          // alpha 채널에는 specular 값이 들어간다
}