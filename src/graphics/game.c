#include "graphics.h"
#include "load_obj.h"
#include "path.h"


void initCamera(World* world){
  memset(&(world->camera), 0, sizeof(world->camera));
  world->camera.pos[0] = 0.0;
  world->camera.pos[1] = -4.0;
  world->camera.pos[2] = 2.0;
  vec3 worldUP;
  worldUP[0] = 0.0;
  worldUP[1] = 0.0;
  worldUP[2] = 1.0;

  vec3 target = {0.0f, 0.0f, 2.0f};
  glm_vec_sub(target, world->camera.pos, world->camera.lookdir);
  glm_normalize(world->camera.lookdir);
  //vec3 origin;
  //glm_vec_zero(origin);
  //glm_vec_sub(origin,world->camera.pos,world->camera.lookdir);
  //glm_vec_normalize(world->camera.lookdir);

  glm_vec_cross(world->camera.lookdir,worldUP,world->camera.right);
  //glm_vec_cross(worldUP, world->camera.lookdir, world->camera.right);
  glm_vec_normalize(world->camera.right);

  //glm_vec_cross(world->camera.right,world->camera.lookdir,world->camera.up);
  glm_vec_cross(world->camera.right,
              world->camera.lookdir,
              world->camera.up);
  glm_vec_normalize(world->camera.up);

  glm_lookat(world->camera.pos,target,world->camera.up, world->camera.view);
  //world->camera.pitch = glm_deg(asinf(world->camera.lookdir[2]));
  //world->camera.yaw   = glm_deg(atan2f(world->camera.lookdir[2], world->camera.lookdir[0]));
  world->camera.pitch = glm_deg(asinf(world->camera.lookdir[2]));
  world->camera.yaw   = glm_deg(atan2f(
				       world->camera.lookdir[1],
				       world->camera.lookdir[0]
				       ));

}


void updateCamera(World* world,float deltaTime){
  vec3 worldUP;
  worldUP[0] = 0.0;
  worldUP[1] = 0.0;
  worldUP[2] = 1.0;
  
  world->camera.yaw   += world->input.mouseDeltaX;
  world->camera.pitch -= world->input.mouseDeltaY;   // subtract because +Y is down

  if (world->camera.pitch > 89.0f)  world->camera.pitch = 89.0f;
  if (world->camera.pitch < -89.0f) world->camera.pitch = -89.0f;

  float yawRad   = glm_rad(world->camera.yaw);
  float pitchRad = glm_rad(world->camera.pitch);

  world->camera.lookdir[0] = cosf(yawRad)*cosf(pitchRad);
  world->camera.lookdir[2] = sinf(pitchRad);
  world->camera.lookdir[1] = sinf(yawRad)*cosf(pitchRad);
  glm_vec_normalize(world->camera.lookdir);

  glm_vec_cross(world->camera.lookdir,worldUP,world->camera.right);
  glm_vec_normalize(world->camera.right);

  glm_vec_cross(world->camera.right,world->camera.lookdir,world->camera.up);
  glm_vec_normalize(world->camera.up);

  vec3 forward;
  glm_vec_scale(world->camera.lookdir,world->input.moveForward*deltaTime,forward);
  glm_vec_add(world->camera.pos,forward,world->camera.pos);

  vec3 right;
  glm_vec_scale(world->camera.right,world->input.moveRight*deltaTime,right);
  glm_vec_add(world->camera.pos,right,world->camera.pos);
  
  vec3 target;
  glm_vec_zero(target);
  glm_vec_add(world->camera.pos,world->camera.lookdir,target);

  glm_lookat(world->camera.pos,target,world->camera.up, world->camera.view);
}

void updateWorld(World* world,float deltaTime){
  updateCamera(world,deltaTime);
}

void createLightViewProj(LightSource* light){
  //TODO: create the light's view proj matrix
  vec3 lightUp = {0, 0, 1};
  vec3 target;
  glm_vec_add(light->pos, light->dir, target);
  mat4 proj,view;
  float near_plane = 1.0f, far_plane = 7.5f;
  //glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f,near_plane,far_plane,proj);
 
  glm_perspective(glm_rad(100.0f), 1.0f, 0.5f, 20.0f, proj);
  //glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 20.0f, proj);
  //glm_ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 30.0f, proj);
  //proj[2][2] *= 0.5f;
  //proj[3][2] = proj[3][2] * 0.5f + 0.5f;
  
  proj[2][2] = proj[2][2] * 0.5f - 0.5f;
  proj[3][2] = proj[3][2] * 0.5f;

  glm_lookat(light->pos,target,lightUp,view);

  glm_mat4_mul(proj,view,light->viewProj);

  

  vec4 p = {0, 0, 5, 1};
  vec4 r;

  glm_mat4_mulv(light->viewProj, p, r);
}

void createLightSources(World* world)
{
    LightSource* lights = world->lightSources;

    vec3 target = { 0.0f, 0.0f, 2.0f };

    // ---------------------------------------------------------
    // Light 0 — front-left, casts shadow
    // ---------------------------------------------------------
    glm_vec_copy((vec3){ -3.0f, -3.0f, 4.0f }, lights[0].pos);
    glm_vec_copy((vec3){ 1.0f, 1.0f, 1.0f }, lights[0].color);
    lights[0].intensity = 0.7f;
    lights[0].has_shadow = 1;

    glm_vec_sub(target, lights[0].pos, lights[0].dir);
    glm_normalize(lights[0].dir);

    
    createLightViewProj(&lights[0]);


    // ---------------------------------------------------------
    // Light 1 — front-right, no shadow
    // ---------------------------------------------------------
    glm_vec_copy((vec3){ 3.0f, -3.0f, 4.0f }, lights[1].pos);
    glm_vec_copy((vec3){ 1.0f, 1.0f, 1.0f }, lights[1].color);
    lights[1].intensity = 0.7f;
    lights[1].has_shadow = 0;

    glm_vec_sub(target, lights[1].pos, lights[1].dir);
    glm_normalize(lights[1].dir);
     createLightViewProj(&lights[1]);


    // ---------------------------------------------------------
    // Light 2 — back-left, no shadow
    // ---------------------------------------------------------
    glm_vec_copy((vec3){ -3.0f, 3.0f, 4.0f }, lights[2].pos);
    glm_vec_copy((vec3){ 1.0f, 1.0f, 1.0f }, lights[2].color);
    lights[2].intensity = 0.7f;
    lights[2].has_shadow = 1;

    glm_vec_sub(target, lights[2].pos, lights[2].dir);
    glm_normalize(lights[2].dir);
     createLightViewProj(&lights[2]);


    // ---------------------------------------------------------
    // Light 3 — back-right, casts shadow
    // ---------------------------------------------------------
    glm_vec_copy((vec3){ 3.0f, 3.0f, 4.0f }, lights[3].pos);
    glm_vec_copy((vec3){ 1.0f, 1.0f, 1.0f }, lights[3].color);
    lights[3].intensity = 0.7f;
    lights[3].has_shadow = 0;

    glm_vec_sub(target, lights[3].pos, lights[3].dir);
    glm_normalize(lights[3].dir);

    createLightViewProj(&lights[3]);


    world->lightCount = 4;
}

void initWorld(World* world, uint32_t width, uint32_t height)
{
  memset(world, 0, sizeof(World));

  world->width = width;
  world->height = height;

  glm_perspective(glm_rad(45.0f),
		  (float)width / (float)height,
		  0.1f,
		  1000.0f,
		  world->proj);

  world->proj[1][1] *= -1.0f;

  initCamera(world);

  world->meshCount = 5;
  world->meshes = malloc(sizeof(Mesh) * world->meshCount);
  memset((world->meshes),0,sizeof(Mesh) * world->meshCount);

  world->meshPbrCount = 1;
  world->meshesPbr = malloc(sizeof(Mesh) * world->meshPbrCount);
  memset((world->meshesPbr),0,sizeof(Mesh) * world->meshPbrCount);
  
  int obj_size;
  char obj_name[] = "Sting.obj";
  char* obj_address =  pathJoin(ASSETS,obj_name,strlen(obj_name),&obj_size);
  world->meshesPbr[0] = loadObjectFile(obj_address);
  glm_vec4_zero(world->meshesPbr[0].quaternion);
  world->meshesPbr[0].quaternion[0] = 1.0;
  mat4 R;
  quatenrionRotation(world->meshesPbr[0].quaternion,R);

  glm_vec_zero(world->meshesPbr[0].position);
  world->meshesPbr[0].position[2] = 0.5;
  world->meshesPbr[0].position[2] = 1.0;
  glm_vec_one(world->meshesPbr[0].scale);
  world->meshesPbr[0].scale[0] = 0.09;
  world->meshesPbr[0].scale[1] = 0.09;
  world->meshesPbr[0].scale[2] = 0.09;
  //glm_vec_scale(world->meshesPbr[0].scale,0.5,world->meshesPbr[0].scale);
  
  world->meshesPbr[0].has_texture = 1;
  world->meshesPbr[0].has_normalMap = 1;
  world->meshesPbr[0].has_metallic = 1;
  world->meshesPbr[0].has_roughness = 1;
  world->meshesPbr[0].has_parallaxMap = 1;
  world->meshesPbr[0].has_ao = 1;
  world->meshesPbr[0].is_pbr = 1;
  world->meshesPbr[0].texture_index = 5 ; world->meshesPbr[0].normal_index = 5;
  world->meshesPbr[0].metallic_index = 0; world->meshesPbr[0].roughness_index = 0; world->meshesPbr[0].ao_index = 0;
  
  mat4 T,S,TR;
  glm_mat4_identity(T);
  glm_translate(T,world->meshesPbr[0].position);

  glm_mat4_identity(S);
  glm_scale(S,world->meshesPbr[0].scale);

  glm_mat4_mul(T,R,TR);
  glm_mat4_mul(TR,S,world->meshesPbr[0].model);

  createFloor(&(world->meshes[0]));
  world->meshes[0].has_texture = 1;
  world->meshes[0].has_normalMap = 1;
  world->meshes[0].has_parallaxMap = 1;
  world->meshes[0].texture_index = 1; world->meshes[0].normal_index = 1; world->meshes[0].parallax_index = 1;
  
  createCeiling(&(world->meshes[1]));
  world->meshes[1].has_texture = 1;
  world->meshes[1].has_normalMap = 1;
  world->meshes[1].has_parallaxMap = 1;
  world->meshes[1].texture_index = 0; world->meshes[1].normal_index = 0; world->meshes[1].parallax_index = 0;
  
  createLWall(&(world->meshes[2]));
  world->meshes[2].has_texture = 1;
  world->meshes[2].has_normalMap = 1;
  world->meshes[2].has_parallaxMap = 1;
  world->meshes[2].texture_index = 2; world->meshes[2].normal_index = 2; world->meshes[2].parallax_index = 2;
  
  createRWall(&(world->meshes[3]));
  world->meshes[3].has_texture = 1;
  world->meshes[3].has_normalMap = 1;
  world->meshes[3].has_parallaxMap = 1;
  world->meshes[3].texture_index = 3; world->meshes[3].normal_index = 3; world->meshes[3].parallax_index = 3;
  
  createFWall(&(world->meshes[4]));
  world->meshes[4].has_texture = 1;
  world->meshes[4].has_normalMap = 1;
  world->meshes[4].has_parallaxMap = 1;
  world->meshes[4].texture_index = 4; world->meshes[4].normal_index = 4; world->meshes[4].parallax_index = 4;


  addMeshTangents(&(world->meshesPbr[0]));
  addMeshTangents(&(world->meshes[0]));
  addMeshTangents(&(world->meshes[1]));
  addMeshTangents(&(world->meshes[2]));
  addMeshTangents(&(world->meshes[3]));
  addMeshTangents(&(world->meshes[4]));

  createLightSources(world);


  world->render_state = 0x0;
}

void cleanupWorld(World* world)
{
  for (uint32_t i = 0; i < world->meshCount; i++) {
    free(world->meshes[i].vertices);
    free(world->meshes[i].indices);
  }

  free(world->meshes);
}
