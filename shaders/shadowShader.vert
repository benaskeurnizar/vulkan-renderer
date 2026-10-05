#version 450

#define MAX_LIGHTS 32

layout(binding=0) uniform UniformBufferLight{
	mat4 viewProj;
}ubog;

layout(binding=1) uniform UniformBufferMesh{
	mat4 model;
	uint has_texture;
	uint is_pbr;
	uint has_normalMap;
	uint has_metallic;
	uint has_roughness;
	uint has_ao;
}ubom;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

void main(){
     gl_Position = ubog.viewProj*ubom.model*vec4(inPosition,1.0);
}