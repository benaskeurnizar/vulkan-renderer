#version 450

#define MAX_LIGHTS 32

struct LightSource{
  vec3 pos;
  vec3 dir;
  vec3 color;
  float intensity;
};

layout(binding=0) uniform UniformBufferGlobal{
	mat4 view;
	mat4 proj;
	vec3 cam_pos;
	LightSource lightSources[MAX_LIGHTS];
	uint lightCount;
}ubog;


layout(binding=1) uniform UniformBufferMesh{
	mat4 model;
}ubom;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(location = 1) out vec3 worldPos;

void main(){
     gl_Position = ubog.proj* ubog.view * ubom.model * vec4(inPosition, 1.0);
     fragColor = inColor;
     worldPos = inPosition;
}