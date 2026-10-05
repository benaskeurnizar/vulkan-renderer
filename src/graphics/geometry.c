#include "graphics.h"
#include "vk_texture.h"


void multiply_quaternions(vec4 a,vec4 b,vec4 d){
  d[0] = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
  d[1] = a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
  d[2] = a[0]*b[2] - a[1]*b[3] + a[2]*b[0] + a[3]*b[1];
  d[3] = a[0]*b[3] + a[1]*b[2] - a[2]*b[1] + a[3]*b[0];
}

void quatenrionRotation(vec4 q, mat4 rot){
  glm_mat4_identity(rot);
  float w=q[0], x=q[1], y=q[2], z=q[3];

  rot[0][0] = 1.0f-2.0f*(y*y+z*z);
  rot[1][0] = 2.0f*(x*y-w*z);
  rot[2][0] = 2.0f*(x*z+w*y);

  rot[0][1] = 2.0f*(x*y+w*z);
  rot[1][1] = 1.0f-2.0f*(x*x+z*z);
  rot[2][1] = 2.0f*(y*z-w*x);

  rot[0][2] = 2.0f*(x*z-w*y);
  rot[1][2] = 2.0f*(y*z+w*x);
  rot[2][2] = 1.0f-2.0f*(x*x+y*y);
}

void rotateMesh(Mesh* mesh,float angle_deg,vec3 axis){
  float angle_rad = angle_deg * (M_PI / 180.0);
  vec4 delta_q;
  delta_q[0] = cosf(angle_rad/2.0);delta_q[1] = axis[0]*sinf(angle_rad/2.0);
  delta_q[2] = axis[1]*sinf(angle_rad/2.0);
  delta_q[3] = axis[2]*sinf(angle_rad/2.0);

  vec4 temp;
  multiply_quaternions(delta_q,mesh->quaternion,temp);
  glm_vec4_copy(temp,mesh->quaternion);

  //NOTE: normalize it
  float n = sqrtf(temp[0]*temp[0]+temp[1]*temp[1]+temp[2]*temp[2]+temp[3]*temp[3]);
  mesh->quaternion[0]/=n;
  mesh->quaternion[1]/=n;
  mesh->quaternion[2]/=n;
  mesh->quaternion[3]/=n;

  mat4 R;
  quatenrionRotation(mesh->quaternion,R);

  mat4 T,S,TR;
  glm_mat4_identity(T);
  glm_translate(T,mesh->position);

  glm_mat4_identity(S);
  glm_scale(S,mesh->scale);

  glm_mat4_mul(T,R,TR);
  glm_mat4_mul(TR,S,mesh->model);
}


void createCubeMesh(Mesh* mesh){
  const Vertex vertices[] = {
    // Front  (+Z)
    {{-0.5f,-0.5f, 0.5f},{1,0,0},{0,0,1}},
    {{ 0.5f,-0.5f, 0.5f},{0,1,0},{0,0,1}},
    {{ 0.5f, 0.5f, 0.5f},{0,0,1},{0,0,1}},
    {{-0.5f, 0.5f, 0.5f},{1,1,1},{0,0,1}},
    // Back   (-Z)
    {{ 0.5f,-0.5f,-0.5f},{1,0,0},{0,0,-1}},
    {{-0.5f,-0.5f,-0.5f},{0,1,0},{0,0,-1}},
    {{-0.5f, 0.5f,-0.5f},{0,0,1},{0,0,-1}},
    {{ 0.5f, 0.5f,-0.5f},{1,1,1},{0,0,-1}},
    // Right  (+X)
    {{ 0.5f,-0.5f, 0.5f},{1,0,0},{1,0,0}},
    {{ 0.5f,-0.5f,-0.5f},{0,1,0},{1,0,0}},
    {{ 0.5f, 0.5f,-0.5f},{0,0,1},{1,0,0}},
    {{ 0.5f, 0.5f, 0.5f},{1,1,1},{1,0,0}},
    // Left   (-X)
    {{-0.5f,-0.5f,-0.5f},{1,0,0},{-1,0,0}},
    {{-0.5f,-0.5f, 0.5f},{0,1,0},{-1,0,0}},
    {{-0.5f, 0.5f, 0.5f},{0,0,1},{-1,0,0}},
    {{-0.5f, 0.5f,-0.5f},{1,1,1},{-1,0,0}},
    // Top    (+Y)
    {{-0.5f, 0.5f, 0.5f},{1,0,0},{0,1,0}},
    {{ 0.5f, 0.5f, 0.5f},{0,1,0},{0,1,0}},
    {{ 0.5f, 0.5f,-0.5f},{0,0,1},{0,1,0}},
    {{-0.5f, 0.5f,-0.5f},{1,1,1},{0,1,0}},
    // Bottom (-Y)
    {{-0.5f,-0.5f,-0.5f},{1,0,0},{0,-1,0}},
    {{ 0.5f,-0.5f,-0.5f},{0,1,0},{0,-1,0}},
    {{ 0.5f,-0.5f, 0.5f},{0,0,1},{0,-1,0}},
    {{-0.5f,-0.5f, 0.5f},{1,1,1},{0,-1,0}},
  };

  const uint32_t indices[] = {
     0, 1, 2,  2, 3, 0,   // front
     4, 5, 6,  6, 7, 4,   // back
     8, 9,10, 10,11, 8,   // right
    12,13,14, 14,15,12,   // left
    16,17,18, 18,19,16,   // top
    20,21,22, 22,23,20,   // bottom
  };

  mesh->vertexCount = sizeof(vertices) / sizeof(vertices[0]);
  mesh->indexCount  = sizeof(indices) / sizeof(indices[0]);
  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  memcpy(mesh->vertices, vertices, sizeof(Vertex) * mesh->vertexCount);
  memcpy(mesh->indices, indices, sizeof(uint32_t) * mesh->indexCount);
  glm_mat4_identity(mesh->model);
}

void createCubeMeshNN(Mesh* mesh){
  const Vertex vertices[] = {
    // Front
    {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},

    // Back
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
    {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
  };

  const uint16_t indices[] = {
    0,1,2, 2,3,0,
    5,4,7, 7,6,5,
    4,0,3, 3,7,4,
    1,5,6, 6,2,1,
    3,2,6, 6,7,3,
    4,5,1, 1,0,4
  };

  mesh->vertexCount = sizeof(vertices) / sizeof(vertices[0]);
  mesh->indexCount  = sizeof(indices) / sizeof(indices[0]);

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint16_t) * mesh->indexCount);

  memcpy(mesh->vertices,
	 vertices,
	 sizeof(Vertex) * mesh->vertexCount);

  memcpy(mesh->indices,
	 indices,
	 sizeof(uint16_t) * mesh->indexCount);

  glm_mat4_identity(mesh->model);
}

int rayHitsGizmo(vec3 rayOrigin,vec3 rayDir,vec3 hitPos){
  float gizmoRaduis = 0.16;
  float gizmoCenterX = 0.8;
  float gizmoCenterY = -0.8;

  vec3 ro;
  glm_vec_copy(rayOrigin,ro);
  
  ro[0] -= gizmoCenterX;ro[1] -= gizmoCenterY;

  

  float a = (rayDir[0]*rayDir[0]) + (rayDir[1]*rayDir[1]) + (rayDir[2]*rayDir[2]);
  float b = 2*(ro[0]*rayDir[0] + ro[1]*rayDir[1] + ro[2]*rayDir[2]);
  float c = (ro[0]*ro[0]) + (ro[1]*ro[1]) + (ro[2]*ro[2]) - (gizmoRaduis*gizmoRaduis);

  float delta = (b*b)-(4*a*c);
  if(delta < 0){
    return 0;
  }
  if(delta == 0){
    float t = -b/(2*a);
    hitPos[0] = ro[0]+ t*rayDir[0];
    hitPos[1] = ro[1]+ t*rayDir[1];
    hitPos[2] = ro[2]+ t*rayDir[2];
  }else{
    float t = (-b-sqrtf(delta))/(2*a);
    hitPos[0] = ro[0]+ t*rayDir[0];
    hitPos[1] = ro[1]+ t*rayDir[1];
    hitPos[2] = ro[2]+ t*rayDir[2];
  }
  return 1;
}


void createRoom(Mesh* mesh)
{
  mesh->vertexCount = 20;
  mesh->indexCount  = 30;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {

    // Floor (z = 0)
    { {  5.0f, -5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f } },
    { { -5.0f, -5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f } },
    { { -5.0f,  5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f } },
    { {  5.0f,  5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f } },

    // Back wall (y = 5)
    { {  5.0f, 5.0f, 0.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f } },
    { { -5.0f, 5.0f, 0.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f } },
    { { -5.0f, 5.0f, 5.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f } },
    { {  5.0f, 5.0f, 5.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f } },

    // Right wall (x = 5)
    { { 5.0f, -5.0f, 0.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f } },
    { { 5.0f,  5.0f, 0.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f } },
    { { 5.0f,  5.0f, 5.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f } },
    { { 5.0f, -5.0f, 5.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f } },

    // Left wall (x = -5)
    { { -5.0f,  5.0f, 0.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f } },
    { { -5.0f, -5.0f, 0.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f } },
    { { -5.0f, -5.0f, 5.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f } },
    { { -5.0f,  5.0f, 5.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f } },

    // Ceiling (z = 5)
    { {  5.0f,  5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f } },
    { { -5.0f,  5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f } },
    { { -5.0f, -5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f } },
    { {  5.0f, -5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f } },
  };
  uint32_t indices[] = {

    // Floor
    0, 1, 2,
    2, 3, 0,

    // Back
    4, 5, 6,
    6, 7, 4,

    // Left
    8, 9, 10,
    10, 11, 8,

    // Right
    12, 13, 14,
    14, 15, 12,

    // Ceiling
    16, 17, 18,
    18, 19, 16,
  };

  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;
}


void createFloor(Mesh* mesh)
{
  mesh->vertexCount = 4;
  mesh->indexCount  = 6;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {
    { {  5.0f, -5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
    { { -5.0f, -5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
    { { -5.0f,  5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
    { {  5.0f,  5.0f, 0.0f }, { 0.75f, 0.75f, 0.75f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
  };
  uint32_t indices[] = {
    // Floor
    0, 1, 2,
    2, 3, 0,
  };
  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;
}

void createCeiling(Mesh* mesh)
{
  mesh->vertexCount = 4;
  mesh->indexCount  = 6;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {
    { {  5.0f,  5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f } },
    { { -5.0f,  5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f } },
    { { -5.0f, -5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  5.0f, -5.0f, 5.0f }, { 0.80f, 0.80f, 0.80f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 0.0f } },
  };
  uint32_t indices[] = {
    // Ceiling
    0, 1, 2,
    2, 3, 0,
  };
  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;
}

void createLWall(Mesh* mesh)
{
  mesh->vertexCount = 4;
  mesh->indexCount  = 6;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {
    { { -5.0f,  5.0f, 0.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
    { { -5.0f, -5.0f, 0.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
    { { -5.0f, -5.0f, 5.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
    { { -5.0f,  5.0f, 5.0f }, { 0.70f, 0.65f, 0.65f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
  };
  uint32_t indices[] = {
    // Ceiling
    0, 1, 2,
    2, 3, 0,
  };
  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;
}

void createRWall(Mesh* mesh)
{
  mesh->vertexCount = 4;
  mesh->indexCount  = 6;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {
    { { 5.0f, -5.0f, 0.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },
    { { 5.0f,  5.0f, 0.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
    { { 5.0f,  5.0f, 5.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { 5.0f, -5.0f, 5.0f }, { 0.65f, 0.65f, 0.70f }, { -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
  };
  uint32_t indices[] = {
    // Ceiling
    0, 1, 2,
    2, 3, 0,
  };
  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;
}

void createFWall(Mesh* mesh)
{
  mesh->vertexCount = 4;
  mesh->indexCount  = 6;

  mesh->vertices = malloc(sizeof(Vertex) * mesh->vertexCount);
  mesh->indices  = malloc(sizeof(uint32_t) * mesh->indexCount);
  Vertex vertices[] = {
    { {  5.0f, 5.0f, 0.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } },
    { { -5.0f, 5.0f, 0.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } },
    { { -5.0f, 5.0f, 5.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } },
    { {  5.0f, 5.0f, 5.0f }, { 0.70f, 0.70f, 0.70f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } },
  };
  uint32_t indices[] = {
    // Ceiling
    0, 1, 2,
    2, 3, 0,
  };
  memcpy(mesh->vertices, vertices, sizeof(vertices));
  memcpy(mesh->indices, indices, sizeof(indices));

  glm_mat4_identity(mesh->model);
  //mesh->model = GLM_MAT4_IDENTITY_INIT;

  glm_vec_zero(mesh->position);
  glm_vec_one(mesh->scale);

  mesh->quaternion[0] = 0.0f;
  mesh->quaternion[1] = 0.0f;
  mesh->quaternion[2] = 0.0f;
  mesh->quaternion[3] = 1.0f;

}

static inline void vec2_sub(const vec2 a, const vec2 b, vec2 dest){
  dest[0] = a[0] - b[0];
  dest[1] = a[1] - b[1];
}


void addMeshTangents(Mesh* mesh){
  for(uint32_t i=0;i<mesh->vertexCount;i++){
    glm_vec_zero(mesh->vertices[i].tangent);
  }
  for(uint32_t i=0;i<mesh->indexCount;i+=3){
    uint32_t idx1 = mesh->indices[i];
    uint32_t idx2 = mesh->indices[i+1];
    uint32_t idx3 = mesh->indices[i+2];
    
    vec3 edge1 ; 
    vec3 edge2 ;
    glm_vec_sub(mesh->vertices[idx2].pos,mesh->vertices[idx1].pos,edge1);
    glm_vec_sub(mesh->vertices[idx3].pos,mesh->vertices[idx1].pos,edge2);

    vec2 deltaUV1 ;
    vec2 deltaUV2;
    vec2_sub(mesh->vertices[idx2].texCoord,mesh->vertices[idx1].texCoord,deltaUV1);
    vec2_sub(mesh->vertices[idx3].texCoord,mesh->vertices[idx1].texCoord,deltaUV2);

    float f = 1.0/(deltaUV1[0]*deltaUV2[1] - deltaUV2[0]*deltaUV1[1]);

    vec3 tangent;
    tangent[0] = f*(deltaUV2[1]*edge1[0] - deltaUV1[1]*edge2[0]);
    tangent[1] = f*(deltaUV2[1]*edge1[1] - deltaUV1[1]*edge2[1]);
    tangent[2] = f*(deltaUV2[1]*edge1[2] - deltaUV1[1]*edge2[2]);

    glm_vec_add(mesh->vertices[idx1].tangent,tangent,mesh->vertices[idx1].tangent);
    glm_vec_add(mesh->vertices[idx2].tangent,tangent,mesh->vertices[idx2].tangent);
    glm_vec_add(mesh->vertices[idx3].tangent,tangent,mesh->vertices[idx3].tangent);
  }

  for(uint32_t i=0;i<mesh->vertexCount;i++){
    float d = glm_vec_dot(mesh->vertices[i].normal,mesh->vertices[i].tangent);
    mesh->vertices[i].tangent[0] -= mesh->vertices[i].normal[0]*d;
    mesh->vertices[i].tangent[1] -= mesh->vertices[i].normal[1]*d;
    mesh->vertices[i].tangent[2] -= mesh->vertices[i].normal[2]*d;

    glm_vec_normalize(mesh->vertices[i].tangent);
  }
}
