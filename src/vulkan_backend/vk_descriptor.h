#pragma once

#include "vk_types.h"
#include "vk_device.h"
#include "vk_buffer.h"
#include "vk_swapChain.h"
#include "graphics.h"
#include "vk_texture.h"

typedef struct {
  VkDescriptorSetLayout layout;
  VkDescriptorSetLayout layoutPBR;
  VkDescriptorSetLayout shadowLayout;
  VkDescriptorPool pool;
  VkDescriptorSet* sets;   // array, size MAX_FRAMES_IN_FLIGHT
  VkDescriptorSet* shadowSets;
  VkDescriptorSet* pbrSets;
} DescriptorCtx;


void createDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t shadowCount);
void createShadowDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);
void createDescriptorLayoutPBR(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t shadowCount);
void createDescriptorPoolsSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount,uint32_t meshCountPbr, FrameUniforms* globalUniform,FrameUniforms* lightUniforms,uint32_t shadowImagesCount);
void cleanDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);
void cleanupDescriptorPoolSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx);

void updateDescriptorSetsSampler(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,uint32_t meshCount);
void updateDescriptorSetsSamplerPbr(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,uint32_t meshCount);
void updateDescriptorSetsTextures(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* textures,TextureCtx* normalMaps,TextureCtx* parallaxMaps,VkSampler textureSampler);

