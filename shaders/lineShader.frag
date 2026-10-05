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

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

layout(location = 1) in vec3 worldPos;

void main(){
     float distanceToCamera = length(worldPos-ubog.cam_pos);
     float fadeStart = 6.0;
     float fadeEnd   = 20.0;
     //float alpha = 1.0 - smoothstep(fadeStart,fadeEnd,distanceToCamera);
     float t = smoothstep(fadeStart, fadeEnd, distanceToCamera);
     float alpha = (1.0 - t) * (1.0 - t);   // steeper falloff
     outColor = vec4(fragColor,alpha);
     
}