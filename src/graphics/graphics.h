#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "cglm.h"
#include <vulkan/vulkan.h>
//#include "../platform/window.h"
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>


#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_LIGHTS 32

typedef struct Input {
  // Button/key state
  uint32_t keys;

  // Current mouse positions
  double mouse_xpos,mouse_ypos;
  int first_mouse_press;
  
  // Mouse movement this frame
  double mouseDeltaX;
  double mouseDeltaY;

  // Movement input this frame (-1..1)
  float moveForward;
  float moveRight;
} InputState;


typedef struct{
  vec3 pos;
  vec3 color;
  vec3 normal;
  vec2 texCoord;
  vec3 tangent;
}Vertex;

typedef struct {
  float position[3];
  vec3 color;
} LineVertex;

typedef struct{
  vec3 pos;
  vec3 dir;
  vec3 color;
  float intensity;
  mat4 viewProj;
  uint32_t has_shadow;
}LightSource;


typedef struct{
  mat4 view;
  mat4 proj;
  vec3 cam_pos;
  LightSource lightSources[MAX_LIGHTS];
  uint32_t lightCount;
}UniformBufferGlobal;

typedef struct{
  mat4 model;
  int has_texture;
  int has_normalMap;
  int has_parallaxMap;
  int has_metallic;
  int has_roughness;
  int has_ao;
  int is_pbr;
}UniformBufferMesh;

typedef struct  {
    mat4 lightViewProj;
}UniformBufferLight;



typedef struct {
  Vertex* vertices;
  uint32_t vertexCount;

  uint32_t* indices;
  uint32_t indexCount;
  
  mat4 model;
  vec3 position;
  vec3 scale;
  vec4 quaternion;

  int has_texture;
  int texture_index;
  
  int is_pbr;
  
  int has_normalMap;
  int normal_index;

  int has_parallaxMap;
  int parallax_index;
  
  int has_metallic;
  int metallic_index;
  
  int has_roughness;
  int roughness_index;
  
  int has_ao;
  int ao_index;
} Mesh;

typedef struct{
  vec3 pos;
  vec3 lookdir;
  vec3 up;
  vec3 right;
  mat4 view;
  float yaw;
  float pitch;
}Camera;

typedef struct {
  Camera camera;
  InputState input;
  mat4 proj;
  int width;
  int height;
  
  Mesh* meshes;
  uint32_t meshCount;

  Mesh* meshesPbr;
  uint32_t meshPbrCount;
  
  uint32_t resized;

  LightSource lightSources[MAX_LIGHTS];
  uint32_t lightCount;

  uint32_t render_state;
} World;

VkVertexInputBindingDescription getBindingDescription();
void getAttributeDescriptions(VkVertexInputAttributeDescription* out);

void initWorld(World* world, uint32_t width, uint32_t height);
  
void updateWorld(World* world,float deltaTime);
void quatenrionRotation(vec4 q,mat4 rot);
void rotateMesh(Mesh* mesh,float angle,vec3 axis);


void getTriangleAttributeDescriptions(VkVertexInputAttributeDescription* out);
VkVertexInputBindingDescription getTriangleBindingDescription();

void getLineAttributeDescriptions(VkVertexInputAttributeDescription* out);

VkVertexInputBindingDescription getLineBindingDescription();

void getGizmoAttributeDescriptions(VkVertexInputAttributeDescription* out);


VkVertexInputBindingDescription getGizmoBindingDescription();


int rayHitsGizmo(vec3 rayOrigin,vec3 rayDir,vec3 hitPos);


void arcballProject(float mouseNdcX, float mouseNdcY, vec3 hitPos);

void createRoom(Mesh* mesh);
void createFloor(Mesh* mesh);
void createCeiling(Mesh* mesh);
void createLWall(Mesh* mesh);
void createRWall(Mesh* mesh);
void createFWall(Mesh* mesh);

void addMeshTangents(Mesh* mesh);
