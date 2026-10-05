#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(set = 0, binding = 1) uniform ModelUBO {
    mat4 model;
} ubo;

layout(push_constant) uniform PC {
    float aspect; // height/width
} pc;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPos;

float fixedScreenX = 0.8;
float fixedScreenY = 0.8;
float fixedScale = 1.0;

void main()
{
    vec4 localPos = ubo.model * vec4(inPosition, 1.0);
    vec2 pos = localPos.xy * fixedScale;
    pos.x *= pc.aspect;
    gl_Position = vec4(pos + vec2(fixedScreenX,fixedScreenY),(localPos.z + 1.0) * 0.5,1.0);
    

    vec3 normal = normalize(inPosition);
    float ndotl = max(dot(normal, normalize(vec3(0.4, 0.7, 0.6))), 0.15); // 0.15 = ambient floor
    fragColor = vec3(0.3) * ndotl;
    fragPos = inPosition;
    //fragPos = vec3(ubo.model * vec4(inPosition, 1.0));
}