#version 450

#define MAX_LIGHTS 32

struct LightSource{
  vec3 pos;
  vec3 dir;
  vec3 color;
  float intensity;
  mat4 viewProj;
  uint has_shadow;
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
	uint has_texture;
	uint is_pbr;
	uint has_normalMap;
	uint has_parallaxMap;
	uint has_metallic;
	uint has_roughness;
	uint has_ao;
}ubom;



layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 tangent;


layout(location=0) out vec3 fragColor;
layout(location=1) out vec3 Normal;
layout(location=2) out vec3 fragPos;
layout(location=3) out vec4 shadowCoord;
layout(location=4) out vec2 texCoord;
layout(location=5) out mat3 TBN;


vec2 positions[3] = vec2[](
     vec2(0.0,-0.5),
     vec2(0.5,0.5),
     vec2(-0.5,0.5)
);


vec3 colors[3] = vec3[](
     vec3(1.0,0.0,0.0),
     vec3(0.0,1.0,0.0),
     vec3(0.0,0.0,1.0)
);


void main(){
     vec3 biTangent = cross(inNormal,tangent);
     vec3 T = normalize(vec3(ubom.model * vec4(tangent, 0.0)));
     vec3 B = normalize(vec3(ubom.model * vec4(biTangent, 0.0)));
     //vec3 N = normalize(vec3(ubom.model * vec4(inNormal,0.0)));
     vec3 N = normalize(mat3(transpose(inverse(ubom.model))) * inNormal);
     TBN = mat3(T, B, N);
     gl_Position = ubog.proj* ubog.view * ubom.model * vec4(inPosition, 1.0);
     fragPos = vec3(ubom.model*vec4(inPosition, 1.0));
     fragColor = inColor;
     //Normal = vec3(ubom.model*vec4(inNormal,1.0));
     Normal = normalize(mat3(transpose(inverse(ubom.model))) * inNormal);
     shadowCoord = ubog.lightSources[0].viewProj*ubom.model*vec4(inPosition,1.0);
     texCoord = inTexCoord;
}

