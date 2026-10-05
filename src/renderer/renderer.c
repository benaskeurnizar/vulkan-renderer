#include "renderer.h"
#include "../platform/window.h"
#include "../vulkan_backend/vk_instance.h"
#include "../vulkan_backend/vk_device.h"
#include "../vulkan_backend/vk_descriptor.h"
#include "../vulkan_backend/vk_command.h"
#include "../vulkan_backend/vk_buffer.h"
#include "../vulkan_backend/vk_descriptor_updates.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "imgui_bridge.h"


void createSurface(Renderer* renderer,Window* window){
  if(glfwCreateWindowSurface(renderer->instanceCtx.instance,(window->window),NULL,&(renderer->surface))!=VK_SUCCESS){
    fprintf(stderr,"failed to create window surface! \n");
    exit(EXIT_FAILURE);
  }
}

void createRendererTextures(Renderer* renderer){
  createTextureSampler(&(renderer->textureSampler),&(renderer->deviceCtx));

  renderer->textureCount = 6;renderer->normalMapsCount = 6;
  renderer->parallaxMapsCount = 6;renderer->metallicMapsCount = 1;
  renderer->roughnessMapsCount = 1;
  renderer->aoMapsCount = 1;//NOTE: DO A DUMMY ONE BECAUSE IT WILL NOT BE USED CURRENTLY
  renderer->textures = (TextureCtx*)calloc(renderer->textureCount,sizeof(TextureCtx));
  renderer->normalMaps = (TextureCtx*)calloc(renderer->normalMapsCount,sizeof(TextureCtx));
  renderer->parallaxMaps = (TextureCtx*)calloc(renderer->parallaxMapsCount,sizeof(TextureCtx));
  renderer->metallicMaps = (TextureCtx*)calloc(renderer->metallicMapsCount,sizeof(TextureCtx));
  renderer->roughnessMaps = (TextureCtx*)calloc(renderer->roughnessMapsCount,sizeof(TextureCtx));
  renderer->aoMaps = (TextureCtx*)calloc(renderer->aoMapsCount,sizeof(TextureCtx));
  createTestTextures((renderer->textures),renderer->textureCount,&(renderer->deviceCtx),&(renderer->commandsCtx));//NOTE: this is same as albedo's
  createTestNormals((renderer->normalMaps),renderer->normalMapsCount,&(renderer->deviceCtx),&(renderer->commandsCtx));
  createTestParallaxes((renderer->parallaxMaps),renderer->parallaxMapsCount,&(renderer->deviceCtx),&(renderer->commandsCtx));
  createTestMetallics((renderer->metallicMaps),renderer->metallicMapsCount,&(renderer->deviceCtx),&(renderer->commandsCtx));
  createTestRoughnessMaps((renderer->roughnessMaps),renderer->roughnessMapsCount,&(renderer->deviceCtx),&(renderer->commandsCtx));
  createTestAoMaps((renderer->aoMaps),renderer->aoMapsCount,&(renderer->deviceCtx),&(renderer->commandsCtx));
}

void initRenderer(Renderer* renderer,Window* window,World* world){
  renderer->currentFrame = 0;
  renderer->framebufferResized = 0;
  createInstanceCtx(&(renderer->instanceCtx));
  createSurface(renderer,window);
  createDeviceCtx(&(renderer->instanceCtx),&(renderer->deviceCtx),renderer->surface);

  uint32_t shadowLightsCount = 0;
  for(uint32_t i=0;i<world->lightCount;i++){
    if(world->lightSources[i].has_shadow==1){
      shadowLightsCount++;
    }
  }
  createSwapChainCtx(&(renderer->swapChainCtx),&(renderer->deviceCtx),window,renderer->surface,shadowLightsCount);
  
  createDescriptorLayout(&(renderer->descriptorsCtx),&(renderer->deviceCtx),shadowLightsCount);
  createShadowDescriptorLayout(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  createDescriptorLayoutPBR(&(renderer->descriptorsCtx),&(renderer->deviceCtx),shadowLightsCount);
  
  createPipelineCtx(&(renderer->pipelineCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx),&(renderer->descriptorsCtx),shadowLightsCount);
  createCommandCtx(&(renderer->commandsCtx),&(renderer->deviceCtx),renderer->surface);

  createRendererTextures(renderer);

  uint32_t totalMeshCount = world->meshCount+world->meshPbrCount;
  renderer->meshBuffersCount = totalMeshCount;
  renderer->meshBuffers = (MeshBuffers*)calloc((int)renderer->meshBuffersCount,sizeof(MeshBuffers));
  //NOTE: the per mesh uniforms
  renderer->meshUniforms = (FrameUniforms*)calloc((int)renderer->meshBuffersCount,sizeof(FrameUniforms));
  VkDeviceSize meshUniformBufferSize = sizeof(UniformBufferMesh);
  int meshBuffersIndex = 0;
  for(uint32_t i=0;i<world->meshPbrCount;i++){
    createMeshBuffers(&(renderer->meshBuffers[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->meshesPbr[i]));
    createUniformBuffers(&(renderer->meshUniforms[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
    meshBuffersIndex++;
  }
  for(uint32_t i=0;i<world->meshCount;i++){
    createMeshBuffers(&(renderer->meshBuffers[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx),&(world->meshes[i]));
    createUniformBuffers(&(renderer->meshUniforms[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx),meshUniformBufferSize);
    meshBuffersIndex++;
  }
  
  //NOTE: the global uniform
  VkDeviceSize globalUniformBufferSize = sizeof(UniformBufferGlobal);
  createUniformBuffers(&(renderer->globalUniform),&(renderer->deviceCtx),&(renderer->commandsCtx),globalUniformBufferSize);

  //NOTE: the light uniform
  VkDeviceSize lightUniformBufferSize = sizeof(UniformBufferLight);
  renderer->lightUniforms = (FrameUniforms*)calloc((int)shadowLightsCount,sizeof(FrameUniforms));
  renderer->lightUniformsCount = shadowLightsCount;
  for(uint32_t i=0;i<shadowLightsCount;i++){
    createUniformBuffers(&(renderer->lightUniforms[i]),&(renderer->deviceCtx),&(renderer->commandsCtx),lightUniformBufferSize);
  }
  
  createDescriptorPoolsSets(&(renderer->descriptorsCtx),&(renderer->deviceCtx),(renderer->meshUniforms),world->meshCount,world->meshPbrCount,&(renderer->globalUniform),(renderer->lightUniforms),shadowLightsCount);

  updateDescriptorSetsSampler(&(renderer->swapChainCtx),&(renderer->deviceCtx),&(renderer->descriptorsCtx),world->meshCount);
  updateDescriptorSetsSamplerPbr(&(renderer->swapChainCtx),&(renderer->deviceCtx),&(renderer->descriptorsCtx),world->meshPbrCount);
  
  updateDescriptorSetsTextures(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshes),world->meshCount,renderer->textures,renderer->normalMaps,renderer->parallaxMaps,renderer->textureSampler);
  updateDescriptorSetsPbrAlbedo(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshesPbr),world->meshPbrCount,renderer->textures,renderer->textureSampler);
  updateDescriptorSetsPbrNormal(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshesPbr),world->meshPbrCount,renderer->normalMaps,renderer->textureSampler);
  updateDescriptorSetsPbrMetallic(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshesPbr),world->meshPbrCount,renderer->metallicMaps,renderer->textureSampler);
  updateDescriptorSetsPbrRoughness(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshesPbr),world->meshPbrCount,renderer->roughnessMaps,renderer->textureSampler);
  updateDescriptorSetsPbrAo(&(renderer->deviceCtx),&(renderer->descriptorsCtx),(world->meshesPbr),world->meshPbrCount,renderer->aoMaps,renderer->textureSampler);
  
  createSyncObjects(&(renderer->syncCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx));
}


void cleanupRenderer(Renderer* renderer,World* world){
  vkDeviceWaitIdle(renderer->deviceCtx.device);
   
  cleanupSyncObjects(&(renderer->syncCtx),&(renderer->swapChainCtx),&(renderer->deviceCtx));
  cleanupDescriptorPoolSets(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  cleanUniformBuffer(&(renderer->globalUniform),&(renderer->deviceCtx));
  uint32_t shadowCount = 0;
  for(uint32_t i=0;i<world->lightCount;i++){
    if(world->lightSources[i].has_shadow){
      shadowCount++;
    }
  }
  for(uint32_t i=0;i<shadowCount;i++){
    cleanUniformBuffer(&(renderer->lightUniforms[i]),&(renderer->deviceCtx));
  }

  uint32_t meshBuffersIndex = 0;
  for (uint32_t i = 0; i < world->meshPbrCount; i++) {
    cleanUniformBuffer(&(renderer->meshUniforms[meshBuffersIndex]),&(renderer->deviceCtx));
    cleanMeshBuffers(&(renderer->meshBuffers[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx));
    meshBuffersIndex++;
  }

  
  for (uint32_t i = 0; i < world->meshCount; i++) {
    cleanUniformBuffer(&(renderer->meshUniforms[meshBuffersIndex]),&(renderer->deviceCtx));
    cleanMeshBuffers(&(renderer->meshBuffers[meshBuffersIndex]),&(renderer->deviceCtx),&(renderer->commandsCtx));
    meshBuffersIndex++;
  }
  
  free(renderer->meshUniforms);

  
  cleanupCommandCtx(&(renderer->commandsCtx),&(renderer->deviceCtx));
  cleanupPipelineCtx(&(renderer->pipelineCtx),&(renderer->deviceCtx));
  cleanDescriptorLayout(&(renderer->descriptorsCtx),&(renderer->deviceCtx));
  
  cleanUpTextures(renderer->textures,renderer->textureCount,&(renderer->deviceCtx));
  cleanUpTextures(renderer->parallaxMaps,renderer->parallaxMapsCount,&(renderer->deviceCtx));
  cleanUpTextures(renderer->normalMaps,renderer->normalMapsCount,&(renderer->deviceCtx));
  cleanUpTextures(renderer->metallicMaps,renderer->metallicMapsCount,&(renderer->deviceCtx));
  cleanUpTextures(renderer->roughnessMaps,renderer->roughnessMapsCount,&(renderer->deviceCtx));
  cleanUpTextures(renderer->aoMaps,renderer->aoMapsCount,&(renderer->deviceCtx));

  cleanUpTexturesSampler(&(renderer->deviceCtx),renderer->textureSampler);
  
  cleanupSwapChainCtx(&(renderer->swapChainCtx),&(renderer->deviceCtx),shadowCount);
  cleanupDeviceCtx(&(renderer->deviceCtx));
  vkDestroySurfaceKHR(renderer->instanceCtx.instance,renderer->surface,NULL);
  cleanupInstanceCtx(&(renderer->instanceCtx));
}

/*


1. Descriptor Set Layout
        ↓
2. Pipeline Layout
        ↓
3. Graphics Pipeline
        ↓
4. Descriptor Pool
        ↓
5. Descriptor Sets
        ↓
6. Update Descriptor Sets
        ↓
7. Render

 */
