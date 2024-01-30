#version 330 core

in vec2 texCoord;
in vec3 position;
out vec3 fragColor;

// 빛의 값을 계산하기 위해 view, lightPos 유니폼으로 받음
uniform vec3 viewPos;
uniform vec3 lightPos;

// texture map은 diffuse map, normal map 2개를 sampler2D로 받음
uniform sampler2D diffuse;
uniform sampler2D normalMap;

void main()
{
    // ambient 계산
    // diffsue: 원래의 color(brickwall.jpg의 컬러)
    vec3 texColor = texture(diffuse, texCoord).xyz;
    // normalMap에서 texture로 값을 가져오면 값의 범위가 0~1사이 값이다 > 이걸 -1 ~ 1 사이로 늘려준다
    // normal map은 이미지로 저장되어 있는데, 저장해야하는 normal vector는 normalize 하더라도 -1 ~ 1 사이로 맞춰야 한다
    // unit vector가 아닐수 있기 때문에 normalize()
    vec3 pixelNorm = normalize((texture(normalMap, texCoord).xyz * 2.0 - 1.0));
    vec3 ambient = texColor * 0.2;

    // diffuse 계산
    vec3 lightDir = normalize(lightPos - position);
    float diff = max(dot(pixelNorm, lightDir), 0.0);
    vec3 diffuse = diff * texColor * 0.8;

    // specular 계싼
    vec3 viewDir = normalize(viewPos - position);
    vec3 reflectDir = reflect(-lightDir, pixelNorm);
    float spec = pow(max(dot(viewDir, reflect), 0.0), 32);
    vec3 specular = spec * vec3(0.5);

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}