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

layout(set =0 ,binding=0) uniform UniformBufferGlobal{
	mat4 view;	  
	mat4 proj;	  
	vec3 cam_pos;
	LightSource lightSources[MAX_LIGHTS];
	uint lightCount;
}ubog;

layout(set = 0,binding=1) uniform UniformBufferMesh{
	mat4 model;
	uint has_texture;
	uint is_pbr;
	uint has_normalMap;
	uint has_metallic;
	uint has_roughness;
	uint has_ao;
}ubom;

layout(set = 0, binding = 2) uniform sampler2D shadowMap[2];
layout(set = 0, binding = 3) uniform sampler2D textureImage;
layout(set = 0, binding = 4) uniform sampler2D normalMap;
layout(set = 0, binding = 5) uniform sampler2D metallicMap;
layout(set = 0, binding = 6) uniform sampler2D roughnessMap;
layout(set = 0, binding = 7) uniform sampler2D aoMap;

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec4 shadowCoord;
layout(location = 4) in vec2 texCoord;
layout(location = 5) in mat3 TBN;

const float PI = 3.14159265359;

vec4 offset_lookup(vec3 loc,vec2 offset,uint index){
     float texmapscale = 1.0 / 2048.0;
     return texture(shadowMap[index],loc.xy+offset * texmapscale);
}

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,uint lightIndex,uint shadowMapIndex)
{
	vec4 shCoord = ubog.lightSources[lightIndex].viewProj*vec4(fragPos,1.0);
	vec3 projCoords = shCoord.xyz/shCoord.w;
	projCoords.xy = projCoords.xy * 0.5 + 0.5;
	float currentDepth = projCoords.z;
	float closestDepth = texture(shadowMap[shadowMapIndex],projCoords.xy).r;

	float shadowCoeff = 0.0;
	
	float bias = max(0.05*(1.0-dot(normal,ubog.lightSources[lightIndex].dir)),0.005);
	closestDepth = offset_lookup(projCoords,vec2(-1.5,0.5),shadowMapIndex).r;
	shadowCoeff += (currentDepth-bias) > closestDepth ? 1.0 : 0.0;

	closestDepth = offset_lookup(projCoords,vec2(0.5,0.5),shadowMapIndex).r;
	shadowCoeff += (currentDepth-bias) > closestDepth ? 1.0 : 0.0;

	closestDepth = offset_lookup(projCoords,vec2(-1.5,-1.5),shadowMapIndex).r;
	shadowCoeff += (currentDepth-bias) > closestDepth ? 1.0 : 0.0;

	closestDepth = offset_lookup(projCoords,vec2(0.5,-1.5),shadowMapIndex).r;
	shadowCoeff += (currentDepth-bias) > closestDepth ? 1.0 : 0.0;

	shadowCoeff /= 4.0;
	return shadowCoeff;
}

float DistributionGGX(vec3 N,vec3 H,float a){
      float a2 = a*a;
      float NdotH = max(dot(N,H),0.0);
      float NdotH2 = NdotH*NdotH;
      float nom = a2;
      float denom = NdotH2*(a2-1.0)+1.0;
      denom = PI * denom * denom;
      return nom/denom;
}

float GeometrySchlickGGX(float NdotV,float k){
      float nom = NdotV;
      float denom = NdotV * (1.0 - k) +k;
      return nom / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
     //vec3 F0 = vec3(0.04);
     //F0 = mix(F0, surfaceColor.rgb, metalness);
     return F0 + (1.0 - F0) * pow(1.0 - cosTheta,5.0);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k){
      float NdotV = max(dot(N,V),0.0);
      float NdotL = max(dot(N,L),0.0);
      float ggx1 = GeometrySchlickGGX(NdotV,k);
      float ggx2 = GeometrySchlickGGX(NdotL,k);

      return ggx1*ggx2;
}


void main(){

     float lightScale = 35.0;
     const vec3 lightPositions[4] = vec3[](
    vec3(-4.0, -4.0, 4.5),
    vec3( 4.0, -4.0, 4.5),
    vec3(-4.0,  -4.0, 2.5),
    vec3( 4.0,  -4.0, 2.5)
);

    const vec3 lightColors[4] = vec3[](
    	  vec3(45.0),
    	  vec3(45.0),
    	  vec3(45.0),
    	  vec3(45.0)
	  );

	   

     vec3 albedo = texture(textureImage, texCoord).rgb;
     vec3 normal = texture(normalMap,texCoord).rgb ;
     normal = normalize(normal * 2.0 -1);
     mat3 TBN_norm = mat3(normalize(TBN[0]), normalize(TBN[1]), normalize(TBN[2]));
     normal = normalize(TBN_norm * normal);

     float metallic = texture(metallicMap,texCoord).r;
     float roughness = texture(roughnessMap,texCoord).r;
     float ao = texture(aoMap,texCoord).r;
     if(ubom.has_ao==0){
	ao = 1.0;
     }


     vec3 N = normalize(Normal);
     vec3 V = normalize(ubog.cam_pos- fragPos);

     vec3 F0 = vec3(0.04);
     F0 = mix(F0,albedo,metallic);

     vec3 Lo = vec3(0.0);
     for(int i=0;i<ubog.lightCount;++i){
     	     vec3 L = normalize(ubog.lightSources[i].pos-fragPos);
	     vec3 H = normalize(V+L);

	     float distance = length(ubog.lightSources[i].pos-fragPos);
	     float attenuation = 1.0/(distance*distance);
	     vec3 radiance = ubog.lightSources[i].color*lightScale*attenuation;

	     vec3 F = fresnelSchlick(max(dot(H,V),0.0), F0);
	     float NDF = DistributionGGX(N,H,roughness);
	     float G = GeometrySmith(N,V,L,roughness);

	     vec3 numerator = NDF*G*F;
	     float denominator = 4.0 * max(dot(N,V),0.0) * max(dot(N,L),0.0);
	     vec3 specular = numerator/max(denominator,0.001);

	     vec3 kS = F;
	     vec3 kD = vec3(1.0) - kS;

	     kD *= 1.0 - metallic;
	     
	     float NdotL = max(dot(N,L),0.0);
	     Lo += (kD * albedo / PI + specular) * radiance * NdotL;
     }
     vec3 ambient = vec3(0.03) * albedo * ao;
     vec3 color = ambient+Lo;
     color = color / (color+vec3(1.0));
     //color = pow(color,vec3(1.0/2.2));

     outColor =  vec4(color,1.0);
}