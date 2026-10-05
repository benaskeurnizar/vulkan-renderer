#pragma once

#include "vk_descriptor.h"

void updateDescriptorSetsPbrAlbedo(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* albedos,VkSampler textureSampler);

void updateDescriptorSetsPbrNormal(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* normalMaps,VkSampler textureSampler);

void updateDescriptorSetsPbrMetallic(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* metallicMaps,VkSampler textureSampler);

void updateDescriptorSetsPbrRoughness(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* roughnessMaps,VkSampler textureSampler);

void updateDescriptorSetsPbrAo(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* aoMaps,VkSampler textureSampler);
