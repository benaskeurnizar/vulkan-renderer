#include "vk_descriptor_updates.h"

void updateDescriptorSetsPbrAlbedo(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* albedos,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_texture ? meshes[i].texture_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = albedos[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 3;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}

void updateDescriptorSetsPbrNormal(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* normalMaps,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_normalMap ? meshes[i].normal_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = normalMaps[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 4;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}


void updateDescriptorSetsPbrMetallic(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* metallicMaps,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_metallic ? meshes[i].metallic_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = metallicMaps[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 5;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}

void updateDescriptorSetsPbrRoughness(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* roughnessMaps,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_roughness ? meshes[i].roughness_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = roughnessMaps[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 6;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}

void updateDescriptorSetsPbrAo(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* aoMaps,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_ao ? meshes[i].ao_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = aoMaps[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 7;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}
