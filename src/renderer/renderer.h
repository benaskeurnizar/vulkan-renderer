#pragma once
#include <stdint.h>
//TODO: include vulkan stuff
#include "../core/file.h"
#include "../platform/window.h"
#include "../graphics/graphics.h"
#include "../vulkan_backend/vk_instance.h"
#include "../vulkan_backend/vk_device.h"
#include "../vulkan_backend/vk_swapChain.h"
#include "../vulkan_backend/vk_sync.h"
#include "../vulkan_backend/vk_pipeline.h"
#include "../vulkan_backend/vk_command.h"
#include "../vulkan_backend/vk_descriptor.h"
#include "../vulkan_backend/vk_buffer.h"

#include "vk_texture.h"


typedef struct{
  VkInstanceCtx instanceCtx;
  VkDeviceCtx deviceCtx;
  VkSurfaceKHR surface;          // or wherever you decide this belongs
  VkSwapchainCtx swapChainCtx;
  VkSyncCtx syncCtx;
  VkPipelineCtx pipelineCtx;
  CommandCtx commandsCtx;
  DescriptorCtx descriptorsCtx;
  
  MeshBuffers* meshBuffers;               // will need to become an array/list once you have >1 mesh
  uint32_t meshBuffersCount;
  FrameUniforms* meshUniforms;

  FrameUniforms globalUniform;

  FrameUniforms* lightUniforms;
  uint32_t lightUniformsCount;
  
  uint32_t currentFrame;
  uint32_t framebufferResized;

  VkSampler textureSampler;

  TextureCtx* textures; uint32_t textureCount;
  TextureCtx* normalMaps; uint32_t normalMapsCount;
  TextureCtx* parallaxMaps; uint32_t parallaxMapsCount;
  TextureCtx* metallicMaps; uint32_t metallicMapsCount;
  TextureCtx* roughnessMaps; uint32_t roughnessMapsCount;
  TextureCtx* aoMaps; uint32_t aoMapsCount;
  
}Renderer;

void initRenderer(Renderer* renderer,Window* window,World* world);

void cleanupRenderer(Renderer* renderer,World* world);
