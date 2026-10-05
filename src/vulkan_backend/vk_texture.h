#pragma once

#include "vk_types.h"
#include "vk_device.h"
#include "vk_command.h"

#include "load_textures.h"

typedef struct{
  int width,height,channels;
  VkImage textureImage;
  VkImageView textureImageView;
  VkDeviceMemory textureImageMemory;
}TextureCtx;

void createTestTextures(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx);
void createTestNormals(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx);
void createTextureSampler(VkSampler* sampler, VkDeviceCtx* deviceCtx);
void cleanUpTextures(TextureCtx* textures,uint32_t textureCount,VkDeviceCtx* deviceCtx);

void cleanUpTexturesSampler(VkDeviceCtx* deviceCtx,VkSampler textureSampler);

void createTestMetallics(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx);

void createTestRoughnessMaps(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx);

void createTestAoMaps(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx);
