#version 330 core

layout (location = 0) in vec3 aPos;
out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main() 
{
    localPos = aPos;
    // view matrix가 4x4였는데, 이를 3x3으로 바꾸면 translation 부분이 제거되고, rotation 파트만 남는다
    // 그리고 다시 4x4로 바꾸면 rotaion만 남은 view matrix가 된다
    // 이 결과로 카메라가 어디에 있던 가운데를 기준으로 skybox를 그리는 역할을 한다
    // 이전에는 skybox를 그릴 때, camera position을 기준으로 움직여서 그림을 그렸다
    mat4 rotView = mat4(mat3(view));
    vec4 clipPos = projection * rotView * vec4(localPos, 1.0);
    // fragment shader로 넘어갈 때, w로 전체 xyz값을 나눠서 normalize를 하는데, ww가 들어가면 z값은 항상 1이 된다
    // 1이 들어갔다는 의미는 depth buffer에서 제일 멀리 있다는 의미이다
    // 대신 depth buffer를 초기화할 때 1로 초기화하고 여기에 그려지는 픽셀도 depth 값이 1이기 때문에 depth_test_less로 설정된 depth test를 통과하지 못한다
    // 따라서 그림을 그리기 전에 depth_test의 기준을 depth_test_less_or_equal로 변경해야 한다
    gl_Position = clipPos.xyww;
}