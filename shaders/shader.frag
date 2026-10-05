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
	uint has_parallaxMap;
	uint has_metallic;
	uint has_roughness;
	uint has_ao;
}ubom;

layout(set = 0, binding = 2) uniform sampler2D shadowMap[2];
layout(set = 0, binding = 3) uniform sampler2D textureImage;
layout(set = 0, binding = 4) uniform sampler2D normalMap;
layout(set = 0, binding = 5) uniform sampler2D parallaxMap;



layout(location = 0) out vec4 outColor;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 fragPos;
layout(location = 3) in vec4 shadowCoord;
layout(location = 4) in vec2 texCoord;
layout(location = 5) in mat3 TBN;


vec3 calculateAmbient(){
     float ambientStrength = 0.1;
     vec3 ambient = vec3(0.0);
     for(uint i=0;i<ubog.lightCount;i++){
     	      ambient += ambientStrength*ubog.lightSources[i].color;
     }
     return ambient;//*fragColor;
}

vec3 calculateDiffuse(vec3 rgb_normal){
     vec3 norm = normalize(rgb_normal);
     vec3 diffuse = vec3(0.0);
     for(uint i=0;i<ubog.lightCount;i++){
     	      float intensity = ubog.lightSources[i].intensity;
     	      vec3 toLight = ubog.lightSources[i].pos - fragPos;
	      float distance = length(toLight);
	      float attenuation =
              1.0 / (1.0 +
              	     0.09 * distance +
                     0.032 * distance * distance);
     	      vec3 lightDir = normalize(ubog.lightSources[i].pos - fragPos);
	      float diffuseStrength = max(dot(norm,lightDir),0.0);
	      
	      diffuse += diffuseStrength*attenuation*ubog.lightSources[i].color* intensity;
     }
     return diffuse;
}

vec3 calculateSpecular(vec3 rgb_normal){
     vec3 norm = normalize(rgb_normal);
     float shininess = 4.0;
     float specularStrength = 1.0;
     vec3 viewDir = normalize(ubog.cam_pos - fragPos);
     vec3 specular = vec3(0.0);
     for(uint i=0;i<ubog.lightCount;i++){
     	      vec3 lightColor = ubog.lightSources[i].color;
	      float intensity = ubog.lightSources[i].intensity;
     	      vec3 toLight = ubog.lightSources[i].pos - fragPos;
	      float distance = length(toLight);
     	      vec3 lightDir = normalize(ubog.lightSources[i].pos - fragPos);
	      vec3 halfWayDir = normalize(lightDir+viewDir);
	      float attenuation = 1.0 / (1.0 + 0.09*distance + 0.032*distance*distance);
     	      normalize(ubog.lightSources[i].dir);		
     	      vec3 reflectDir = reflect(-ubog.lightSources[i].dir,norm);
	      //float spec = pow(max(dot(viewDir,reflectDir),0.0),32);
	      float spec = pow(max(dot(norm,halfWayDir),0.0),shininess);
	      specular += (specularStrength*spec*lightColor*attenuation*intensity);
     }
     return specular;
}

vec4 offset_lookup(vec3 loc,vec2 offset,uint index){
     float texmapscale = 1.0 / 2048.0;
     return texture(shadowMap[index],loc.xy+offset * texmapscale);
}

float ShadowCalculation2(vec4 fragPosLightSpace,vec3 normal,uint lightIndex,uint shadowMapIndex)
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

float ShadowCalculation(
    vec4 fragPosLightSpace,
    vec3 normal,
    uint lightIndex,
    uint shadowMapIndex)
{
   
    vec4 shCoord =
        ubog.lightSources[lightIndex].viewProj * vec4(fragPos, 1.0);

    vec3 projCoords = shCoord.xyz / shCoord.w;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    // Outside shadow map -> not shadowed
    if (projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0 ||
        projCoords.z < 0.0 || projCoords.z > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    vec3 lightDir =
        normalize(ubog.lightSources[lightIndex].pos - fragPos);

    float bias = max(
        0.05 * (1.0 - dot(normal, lightDir)),
        0.005
    );

    float shadowCoeff = 0.0;

    float texelSize = 1.0 / 2048.0;

    shadowCoeff +=
        (currentDepth - bias >
         texture(shadowMap[shadowMapIndex],
                 projCoords.xy + vec2(-0.5, -0.5) * texelSize).r)
        ? 1.0 : 0.0;

    shadowCoeff +=
        (currentDepth - bias >
         texture(shadowMap[shadowMapIndex],
                 projCoords.xy + vec2( 0.5, -0.5) * texelSize).r)
        ? 1.0 : 0.0;

    shadowCoeff +=
        (currentDepth - bias >
         texture(shadowMap[shadowMapIndex],
                 projCoords.xy + vec2(-0.5,  0.5) * texelSize).r)
        ? 1.0 : 0.0;

    shadowCoeff +=
        (currentDepth - bias >
         texture(shadowMap[shadowMapIndex],
                 projCoords.xy + vec2( 0.5,  0.5) * texelSize).r)
        ? 1.0 : 0.0;

    return shadowCoeff / 4.0;
}

vec2 parallaxMapping(vec2 texCoords,vec3 viewDir){
     float height_scale = 0.01;
     float height = texture(parallaxMap,texCoords).r;
     vec2 p = viewDir.xy / viewDir.z * (height*height_scale);
     return texCoords-p;
}

void main(){
    
     mat3 TBN_inv = transpose(TBN);
     vec3 viewDirWorld = normalize(ubog.cam_pos - fragPos);
     vec3 viewDirTangent = normalize(transpose(TBN) * viewDirWorld);


     vec2 finalTexCoord = texCoord;
     if(ubom.has_parallaxMap == 1){
	finalTexCoord = parallaxMapping(texCoord,viewDirTangent);
     }

     vec3 rgb_normal = normalize(Normal);
     if(ubom.has_normalMap == 1){
	rgb_normal = texture(normalMap,finalTexCoord).rgb ;
     	rgb_normal = normalize(rgb_normal * 2.0 -1);
     	mat3 TBN_norm = mat3(normalize(TBN[0]), normalize(TBN[1]), normalize(TBN[2]));
     	rgb_normal = normalize(TBN_norm * rgb_normal);
     }
     
     uint lightIndex=0;
     uint shadowMapIndex = 0;
     float shadow = 0;
     for(uint i=0;i<ubog.lightCount;i++){
     	      if(ubog.lightSources[i].has_shadow==1){
		shadow += ShadowCalculation(shadowCoord,rgb_normal,i,shadowMapIndex);
		shadowMapIndex++;
	      }
     }
     shadow = min(shadow, 1.0);
     vec3 ambientColor = calculateAmbient();
     vec3 diffuseColor = calculateDiffuse(rgb_normal);
     vec3 specularColor = calculateSpecular(rgb_normal);
     vec3 color;	  
     if(ubom.has_texture==1){
	color = texture(textureImage,finalTexCoord).rgb;
     }else{
	color = fragColor.rgb;
     }
     vec3 result = (ambientColor+(1.0-shadow)*(diffuseColor+specularColor))*color;
   
     outColor = vec4(result,1.0);

  

}
