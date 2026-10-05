#include "vk_descriptor.h"
#include "vk_device.h"


void createDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t shadowCount){
  VkDescriptorSetLayoutBinding uboLayoutBinding1 = {0};
  uboLayoutBinding1.binding = 0;
  uboLayoutBinding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding1.descriptorCount = 1;
  uboLayoutBinding1.stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding1.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding uboLayoutBinding2 = {0};
  uboLayoutBinding2.binding = 1;
  uboLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding2.descriptorCount = 1;
  uboLayoutBinding2.stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding2.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding shadowBinding = {0};
  shadowBinding.binding = 2;
  shadowBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  shadowBinding.descriptorCount = shadowCount;//TODO: as mush as shadoes
  shadowBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding textureBinding = {0};
  textureBinding.binding = 3;
  textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  textureBinding.descriptorCount = 1;
  textureBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalMapBinding = {0};
  normalMapBinding.binding = 4;
  normalMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalMapBinding.descriptorCount = 1;
  normalMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;
  
  VkDescriptorSetLayoutBinding parallaxMapBinding = {0};
  parallaxMapBinding.binding = 5;
  parallaxMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  parallaxMapBinding.descriptorCount = 1;
  parallaxMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding1,uboLayoutBinding2,shadowBinding,textureBinding,normalMapBinding,parallaxMapBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 6;
  layoutInfo.pBindings = bindings; 
  if(vkCreateDescriptorSetLayout(deviceCtx->device,&layoutInfo,NULL,&(descriptorsCtx->layout)) != VK_SUCCESS ){
     fprintf(stderr,"failed to create descriptor set layout !\n");
     exit(EXIT_FAILURE);
  }
}

void createDescriptorLayoutPBR(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t shadowCount){
  VkDescriptorSetLayoutBinding uboLayoutBinding1 = {0};
  uboLayoutBinding1.binding = 0;
  uboLayoutBinding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding1.descriptorCount = 1;
  uboLayoutBinding1.stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding1.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding uboLayoutBinding2 = {0};
  uboLayoutBinding2.binding = 1;
  uboLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding2.descriptorCount = 1;
  uboLayoutBinding2.stageFlags=VK_SHADER_STAGE_VERTEX_BIT|VK_SHADER_STAGE_FRAGMENT_BIT;
  uboLayoutBinding2.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding shadowBinding = {0};
  shadowBinding.binding = 2;
  shadowBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  shadowBinding.descriptorCount = shadowCount;
  shadowBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding albedoBinding = {0};
  albedoBinding.binding = 3;
  albedoBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  albedoBinding.descriptorCount = 1;
  albedoBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding normalMapBinding = {0};
  normalMapBinding.binding = 4;
  normalMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  normalMapBinding.descriptorCount = 1;
  normalMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;
  
  VkDescriptorSetLayoutBinding metallicMapBinding = {0};
  metallicMapBinding.binding = 5;
  metallicMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  metallicMapBinding.descriptorCount = 1;
  metallicMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding roughnessMapBinding = {0};
  roughnessMapBinding.binding = 6;
  roughnessMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  roughnessMapBinding.descriptorCount = 1;
  roughnessMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding aoMapBinding = {0};
  aoMapBinding.binding = 7;
  aoMapBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  aoMapBinding.descriptorCount = 1;
  aoMapBinding.stageFlags=VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding1,uboLayoutBinding2,shadowBinding,albedoBinding,normalMapBinding,metallicMapBinding,roughnessMapBinding,aoMapBinding};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 8;
  layoutInfo.pBindings = bindings; 
  if(vkCreateDescriptorSetLayout(deviceCtx->device,&layoutInfo,NULL,&(descriptorsCtx->layoutPBR)) != VK_SUCCESS ){
     fprintf(stderr,"failed to create descriptor set layout !\n");
     exit(EXIT_FAILURE);
  }
}

void createShadowDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  VkDescriptorSetLayoutBinding uboLayoutBinding1 = {0};
  uboLayoutBinding1.binding = 0;
  uboLayoutBinding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding1.descriptorCount = 1;
  uboLayoutBinding1.stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding1.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding uboLayoutBinding2 = {0};
  uboLayoutBinding2.binding = 1;
  uboLayoutBinding2.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding2.descriptorCount = 1;
  uboLayoutBinding2.stageFlags=VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding2.pImmutableSamplers = NULL;//Optional

  VkDescriptorSetLayoutBinding bindings[] = {uboLayoutBinding1,uboLayoutBinding2};
  VkDescriptorSetLayoutCreateInfo layoutInfo = {0};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 2;
  layoutInfo.pBindings = bindings; 
  if(vkCreateDescriptorSetLayout(deviceCtx->device,&layoutInfo,NULL,&(descriptorsCtx->shadowLayout)) != VK_SUCCESS ){
     fprintf(stderr,"failed to create descriptor set layout !\n");
     exit(EXIT_FAILURE);
  }
}

void createDescriptorPool(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx,uint32_t meshCount,uint32_t shadowImagesCount){
  uint32_t setsPerPass = meshCount * MAX_FRAMES_IN_FLIGHT;
  uint32_t totalSets = setsPerPass * (2+shadowImagesCount); // main + shadow
  //uint32_t descriptorCount = (meshCount) * MAX_FRAMES_IN_FLIGHT*2;//TODO: +1 is for the gizmo...will be updated later.
  uint32_t regularImagesPerSet = shadowImagesCount + 3;
  uint32_t pbrImagesPerSet     = shadowImagesCount + 5;
  VkDescriptorPoolSize poolSizes[2] = {0};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = totalSets*2;

  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = setsPerPass * (regularImagesPerSet + pbrImagesPerSet);

  VkDescriptorPoolCreateInfo poolInfo = {0};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 2;
  poolInfo.pPoolSizes = poolSizes;
  poolInfo.maxSets = totalSets;

  if(vkCreateDescriptorPool(deviceCtx->device,&poolInfo,NULL,&(descriptorsCtx->pool))!=VK_SUCCESS){
    fprintf(stderr,"failed to create descriptor pool!\n");
    exit(EXIT_FAILURE);
  }
}


void createDescriptorSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount, FrameUniforms* globalUniform){
  
  uint32_t descriptorCount = (meshCount) * MAX_FRAMES_IN_FLIGHT;
  //VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
  VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) *descriptorCount);

  for (int i = 0; i < (descriptorCount); i++) {
    layouts[i] = descriptorsCtx->layout;
  }

  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorsCtx->pool;
  allocInfo.descriptorSetCount = (uint32_t)descriptorCount;
  allocInfo.pSetLayouts = layouts;

 
  descriptorsCtx->sets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet)*descriptorCount);
  if(vkAllocateDescriptorSets(deviceCtx->device,&allocInfo,descriptorsCtx->sets)!=VK_SUCCESS){
    fprintf(stderr,"failed to create descriptor sets!\n");
    exit(EXIT_FAILURE);
  }
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      VkDescriptorBufferInfo bufferInfoGlobal = {0};
      bufferInfoGlobal.buffer = globalUniform->uniformBuffers[j];
      bufferInfoGlobal.offset = 0;
      bufferInfoGlobal.range = sizeof(UniformBufferGlobal);

      VkWriteDescriptorSet descriptorWrite = {0};
      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=0;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoGlobal;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

     
      VkDescriptorBufferInfo bufferInfoMesh = {0};
      bufferInfoMesh.buffer = meshUniforms[i].uniformBuffers[j];
      bufferInfoMesh.offset = 0;
      bufferInfoMesh.range = sizeof(UniformBufferMesh);

      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->sets[setIndex];
      descriptorWrite.dstBinding=1;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoMesh;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

      setIndex++;
    }
  }
  

  free(layouts);
}


void createDescriptorSetsPBR(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount, FrameUniforms* globalUniform){
  
  uint32_t descriptorCount = (meshCount) * MAX_FRAMES_IN_FLIGHT;
  
  VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) *descriptorCount);

  for (int i = 0; i < (descriptorCount); i++) {
    layouts[i] = descriptorsCtx->layoutPBR;
  }

  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorsCtx->pool;
  allocInfo.descriptorSetCount = (uint32_t)descriptorCount;
  allocInfo.pSetLayouts = layouts;

 
  descriptorsCtx->pbrSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet)*descriptorCount);
  if(vkAllocateDescriptorSets(deviceCtx->device,&allocInfo,descriptorsCtx->pbrSets)!=VK_SUCCESS){
    fprintf(stderr,"failed to create PBR descriptor sets!\n");
    exit(EXIT_FAILURE);
  }
  //NOTE: first the global
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      VkDescriptorBufferInfo bufferInfoGlobal = {0};
      bufferInfoGlobal.buffer = globalUniform->uniformBuffers[j];
      bufferInfoGlobal.offset = 0;
      bufferInfoGlobal.range = sizeof(UniformBufferGlobal);

      VkWriteDescriptorSet descriptorWrite = {0};
      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->pbrSets[setIndex];
      descriptorWrite.dstBinding=0;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoGlobal;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

     
      VkDescriptorBufferInfo bufferInfoMesh = {0};
      bufferInfoMesh.buffer = meshUniforms[i].uniformBuffers[j];
      bufferInfoMesh.offset = 0;
      bufferInfoMesh.range = sizeof(UniformBufferMesh);

      descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      descriptorWrite.dstSet= descriptorsCtx->pbrSets[setIndex];
      descriptorWrite.dstBinding=1;
      descriptorWrite.dstArrayElement=0;
      descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      descriptorWrite.descriptorCount=1;
      descriptorWrite.pBufferInfo = &bufferInfoMesh;
      descriptorWrite.pImageInfo = NULL; // Optional
      descriptorWrite.pTexelBufferView = NULL; // Optional
      vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

      setIndex++;
    }
  }
}

void updateDescriptorSetsTextures(VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,Mesh* meshes,uint32_t meshCount,TextureCtx* textures,TextureCtx* normalMaps,TextureCtx* parallaxMaps,VkSampler textureSampler){
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      int tex_idx = meshes[i].has_texture ? meshes[i].texture_index : 0;
      //NOTE: the texture
      VkDescriptorImageInfo textureInfo = {0};
      textureInfo.sampler = textureSampler;
      textureInfo.imageView = textures[tex_idx].textureImageView;
      textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->sets[setIndex];
      write.dstBinding = 3;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = 1;
      write.pImageInfo = &textureInfo;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);

      //NOTE: the normal map
      VkDescriptorImageInfo normalMapInfo = {0};
      normalMapInfo.sampler = textureSampler;
      normalMapInfo.imageView = normalMaps[tex_idx].textureImageView;
      normalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      write.dstBinding = 4;
      write.pImageInfo = &normalMapInfo;
      
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);

      //NOTE: the parallax map
      VkDescriptorImageInfo parallaxMapInfo = {0};
      parallaxMapInfo.sampler = textureSampler;
      parallaxMapInfo.imageView = parallaxMaps[tex_idx].textureImageView;
      parallaxMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      write.dstBinding = 5;
      write.pImageInfo = &parallaxMapInfo;

      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
       
      setIndex++;
    }
  }
}

void updateDescriptorSetsSampler(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,uint32_t meshCount){
  uint32_t shadowCount = swapChainCtx->shadowCtx.count;
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
      
      VkDescriptorImageInfo* shadowInfos = (VkDescriptorImageInfo*)calloc(shadowCount,sizeof(VkDescriptorImageInfo));
      for(uint32_t k=0;k<shadowCount;k++){
	shadowInfos[k].sampler = swapChainCtx->shadowCtx.shadowSampler;
	shadowInfos[k].imageView = swapChainCtx->shadowCtx.shadowImageViews[k];
	shadowInfos[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      }
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->sets[setIndex];
      write.dstBinding = 2;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = shadowCount;
      write.pImageInfo = shadowInfos;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
  }
}


void updateDescriptorSetsSamplerPbr(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorsCtx,uint32_t meshCount){
  uint32_t shadowCount = swapChainCtx->shadowCtx.count;
  uint32_t setIndex = 0;
  for(uint32_t i=0;i<meshCount;i++){
    for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
       VkDescriptorImageInfo* shadowInfos = (VkDescriptorImageInfo*)calloc(shadowCount,sizeof(VkDescriptorImageInfo));
      for(uint32_t k=0;k<shadowCount;k++){
	shadowInfos[k].sampler = swapChainCtx->shadowCtx.shadowSampler;
	shadowInfos[k].imageView = swapChainCtx->shadowCtx.shadowImageViews[k];
	shadowInfos[k].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      }
  
      VkWriteDescriptorSet write = {0};
      write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write.dstSet = descriptorsCtx->pbrSets[setIndex];
      write.dstBinding = 2;
      write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
      write.descriptorCount = shadowCount;
      write.pImageInfo = shadowInfos;
      vkUpdateDescriptorSets(deviceCtx->device, 1, &write, 0, NULL);
      setIndex++;
    }
    
  }
}

void createShadowDescriptorSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount, FrameUniforms* lightUniforms,uint32_t shadowCount){

  uint32_t descriptorCount = (meshCount) * MAX_FRAMES_IN_FLIGHT * shadowCount;
  //VkDescriptorSetLayout layouts[MAX_FRAMES_IN_FLIGHT];
  VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*)malloc(sizeof(VkDescriptorSetLayout) *descriptorCount);

  for (int i = 0; i < (descriptorCount); i++) {
    layouts[i] = descriptorsCtx->shadowLayout;
  }

  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = descriptorsCtx->pool;
  allocInfo.descriptorSetCount = (uint32_t)descriptorCount;
  allocInfo.pSetLayouts = layouts;

 
  descriptorsCtx->shadowSets = (VkDescriptorSet*)malloc(sizeof(VkDescriptorSet)*descriptorCount);
  if(vkAllocateDescriptorSets(deviceCtx->device,&allocInfo,descriptorsCtx->shadowSets)!=VK_SUCCESS){
    fprintf(stderr,"failed to create shadow descriptor sets!\n");
    exit(EXIT_FAILURE);
  }
  //NOTE: first the global
  uint32_t setIndex = 0;
  //NOTE: NOW THE MESH UNIFORMS
  for(uint32_t s=0;s<shadowCount;s++){
    for(uint32_t i=0;i<meshCount;i++){
      for(size_t j=0;j<MAX_FRAMES_IN_FLIGHT;j++){
	VkDescriptorBufferInfo bufferInfoGlobal = {0};
	bufferInfoGlobal.buffer = lightUniforms[s].uniformBuffers[j];
	bufferInfoGlobal.offset = 0;
	bufferInfoGlobal.range = sizeof(UniformBufferLight);

	VkWriteDescriptorSet descriptorWrite = {0};
	descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet= descriptorsCtx->shadowSets[setIndex];
	descriptorWrite.dstBinding=0;
	descriptorWrite.dstArrayElement=0;
	descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount=1;
	descriptorWrite.pBufferInfo = &bufferInfoGlobal;
	descriptorWrite.pImageInfo = NULL; // Optional
	descriptorWrite.pTexelBufferView = NULL; // Optional
	vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

     
	VkDescriptorBufferInfo bufferInfoMesh = {0};
	bufferInfoMesh.buffer = meshUniforms[i].uniformBuffers[j];
	bufferInfoMesh.offset = 0;
	bufferInfoMesh.range = sizeof(UniformBufferMesh);

	descriptorWrite.sType=VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet= descriptorsCtx->shadowSets[setIndex];
	descriptorWrite.dstBinding=1;
	descriptorWrite.dstArrayElement=0;
	descriptorWrite.descriptorType=VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount=1;
	descriptorWrite.pBufferInfo = &bufferInfoMesh;
	descriptorWrite.pImageInfo = NULL; // Optional
	descriptorWrite.pTexelBufferView = NULL; // Optional
	vkUpdateDescriptorSets(deviceCtx->device, 1, &descriptorWrite, 0,NULL);

	setIndex++;
      }
    }
  }
  

  free(layouts);
}

void createDescriptorPoolsSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx, FrameUniforms* meshUniforms,uint32_t meshCount,uint32_t meshCountPbr, FrameUniforms* globalUniform,FrameUniforms* lightUniforms,uint32_t shadowImagesCount){
  createDescriptorPool(descriptorsCtx,deviceCtx,meshCount+meshCountPbr,shadowImagesCount);
  createDescriptorSetsPBR(descriptorsCtx,deviceCtx,meshUniforms,meshCountPbr,globalUniform);
  createDescriptorSets(descriptorsCtx,deviceCtx,&(meshUniforms[1]),meshCount,globalUniform);
  //NOTE: the indexes and handles are hardcoded here. 
  createShadowDescriptorSets(descriptorsCtx,deviceCtx,meshUniforms,meshCount+meshCountPbr,lightUniforms,shadowImagesCount);
}


void cleanDescriptorLayout(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  vkDestroyDescriptorSetLayout(deviceCtx->device,descriptorsCtx->layout,NULL);
  vkDestroyDescriptorSetLayout(deviceCtx->device,descriptorsCtx->shadowLayout,NULL);
  vkDestroyDescriptorSetLayout(deviceCtx->device,descriptorsCtx->layoutPBR,NULL);
}


void cleanupDescriptorPoolSets(DescriptorCtx* descriptorsCtx,VkDeviceCtx* deviceCtx){
  vkDestroyDescriptorPool(deviceCtx->device, descriptorsCtx->pool, NULL);
  free(descriptorsCtx->sets);
  free(descriptorsCtx->pbrSets);
  free(descriptorsCtx->shadowSets);
}
