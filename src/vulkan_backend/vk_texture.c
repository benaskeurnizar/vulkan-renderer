#include "vk_texture.h"
#include "vk_buffer.h"
#include "path.h"

void createTextureImage(TextureCtx* texture,VkDeviceCtx* deviceCtx,int width,int height,int normal){
  VkImageCreateInfo imageInfo = {0};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = normal ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateImage(deviceCtx->device,
		    &imageInfo,
		    NULL,
		    &(texture->textureImage)) != VK_SUCCESS) {
    fprintf(stderr, "failed to create texture Image image!\n");
    exit(EXIT_FAILURE);
  }

}

void textureLayoutTransition(TextureCtx* texture, CommandCtx* commandCtx, VkDeviceCtx* deviceCtx,
                              VkImageLayout from, VkImageLayout to){
  printf("[textureLayoutTransition] image = %p, from=%d to=%d\n", (void*)texture->textureImage, from, to);
  VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandCtx, deviceCtx);

  VkImageMemoryBarrier barrier = {0};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.oldLayout = from;
  barrier.newLayout = to;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = texture->textureImage;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  VkPipelineStageFlags sourceStage;
  VkPipelineStageFlags destinationStage;

  if (from == VK_IMAGE_LAYOUT_UNDEFINED && to == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
  } else if (from == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && to == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
  } else {
    fprintf(stderr, "unsupported layout transition!\n");
    exit(EXIT_FAILURE);
  }

  vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0,
                        0, NULL, 0, NULL, 1, &barrier);

  endSingleTimeCommands(&commandBuffer, commandCtx,deviceCtx);
}

void textureLayoutTransition2(TextureCtx* texture,VkDeviceCtx* deviceCtx,VkImageLayout from,VkImageLayout to){
  VkImageSubresourceRange subresourceRange = {0};

  subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  subresourceRange.baseMipLevel = 0;
  subresourceRange.levelCount = 1;
  subresourceRange.baseArrayLayer = 0;
  subresourceRange.layerCount = 1;

  VkHostImageLayoutTransitionInfo transitionInfo = {0};
  transitionInfo.sType = VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO;
  transitionInfo.pNext = NULL;
  transitionInfo.image = texture->textureImage;
  transitionInfo.oldLayout = from;
  transitionInfo.newLayout = to;
  transitionInfo.subresourceRange = subresourceRange;
  if(vkTransitionImageLayout(deviceCtx->device,1,&transitionInfo)!=VK_SUCCESS){
    fprintf(stderr, "failed to perform layout transition!\n");
    exit(EXIT_FAILURE);
  }
}

void createTextureBuffer(TextureCtx* texture,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,unsigned char* pixels){
  VkDeviceSize bufferSize = sizeof(unsigned char)*texture->width*texture->height*4;//texture->channels;
  VkBuffer stagingBuffer = {0};
  VkDeviceMemory stagingBufferMemory = {0};
  createBuffer(deviceCtx,bufferSize,VK_BUFFER_USAGE_TRANSFER_SRC_BIT,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,&stagingBuffer,&stagingBufferMemory);

  //NOTE: Filling the vertex buffer
  // mapping the buffer memory into CPU accessible memory with vkMapMemory.
  void* data;
  vkMapMemory(deviceCtx->device, stagingBufferMemory, 0, bufferSize, 0, &data);
  // This function allows us to access a region of the specified memory resource defined by an offset and size. 
  // The offset and size here are 0 and bufferInfo.size
  memcpy(data,pixels,(size_t)bufferSize);
  vkUnmapMemory(deviceCtx->device, stagingBufferMemory);
  textureLayoutTransition(texture,commandCtx,deviceCtx,VK_IMAGE_LAYOUT_UNDEFINED,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  VkCommandBuffer copyCommand = beginSingleTimeCommands(commandCtx,deviceCtx);
  VkBufferImageCopy region = {0};
  region.bufferOffset = 0;              // where in the buffer to start reading
  region.bufferRowLength = 0;           // 0 = tightly packed, no padding between rows
  region.bufferImageHeight = 0;         // 0 = tightly packed

  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  region.imageSubresource.mipLevel = 0;
  region.imageSubresource.baseArrayLayer = 0;
  region.imageSubresource.layerCount = 1;

  region.imageOffset = (VkOffset3D){0, 0, 0};       // where in the image to start writing
  region.imageExtent = (VkExtent3D){texture->width, texture->height, 1}; // how much to copy
  vkCmdCopyBufferToImage(copyCommand,stagingBuffer,texture->textureImage,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&region);
  endSingleTimeCommands(&copyCommand,commandCtx,deviceCtx);

  textureLayoutTransition(texture,commandCtx,deviceCtx,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
  vkDestroyBuffer(deviceCtx->device,stagingBuffer,NULL);
  vkFreeMemory(deviceCtx->device,stagingBufferMemory,NULL);
}

void createTextureCtx(TextureCtx* texture,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,unsigned char* pixels,int normal);

void createTestTextures(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  //Ceiling
  int size;
  char* texture_address = pathJoin(TEXTURES, "test_ceiling.png", sizeof("test_ceiling.png") - 1, &size);
  unsigned char* ceiling_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,ceiling_pixels,0);
  freeTexturePixels(ceiling_pixels);
  free(texture_address);
  //Floor
  texture_address = pathJoin(TEXTURES, "test_floor.png", sizeof("test_floor.png") - 1, &size);
  unsigned char* floor_pixels = getTexturePixels(texture_address,&(textures[1].width),&(textures[1].height),&(textures[1].channels));
  createTextureCtx(&(textures[1]),deviceCtx,commandCtx,floor_pixels,0);
  freeTexturePixels(floor_pixels);
  free(texture_address);
  //RWall
  texture_address = pathJoin(TEXTURES, "test_wall.png", sizeof("test_wall.png") - 1, &size);
  unsigned char* rwall_pixels = getTexturePixels(texture_address,&(textures[2].width),&(textures[2].height),&(textures[2].channels));
  createTextureCtx(&(textures[2]),deviceCtx,commandCtx,rwall_pixels,0);
  freeTexturePixels(rwall_pixels);
  free(texture_address);
  //LWall
  texture_address = pathJoin(TEXTURES, "test_wall.png", sizeof("test_wall.png") - 1, &size);
  unsigned char* lwall_pixels = getTexturePixels(texture_address,&(textures[3].width),&(textures[3].height),&(textures[3].channels));
  createTextureCtx(&(textures[3]),deviceCtx,commandCtx,lwall_pixels,0);
  freeTexturePixels(lwall_pixels);
  free(texture_address);
  //FWall
  texture_address = pathJoin(TEXTURES, "test_wall.png", sizeof("test_wall.png") - 1, &size);
  unsigned char* fwall_pixels = getTexturePixels(texture_address,&(textures[4].width),&(textures[4].height),&(textures[4].channels));
  createTextureCtx(&(textures[4]),deviceCtx,commandCtx,fwall_pixels,0);
  freeTexturePixels(fwall_pixels);
  free(texture_address);

  //Air_Gun
  texture_address = pathJoin(TEXTURES, "Sting.png", sizeof("Sting.png") - 1, &size);
  unsigned char* sting_pixels = getTexturePixels(texture_address,&(textures[5].width),&(textures[5].height),&(textures[5].channels));
  createTextureCtx(&(textures[5]),deviceCtx,commandCtx,sting_pixels,0);
  freeTexturePixels(sting_pixels);
  free(texture_address);
}

void createTestNormals(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  int size;
  char* texture_address = pathJoin(TEXTURES, "test_ceiling_normal.png", sizeof("test_ceiling_normal.png") - 1, &size);
  unsigned char* ceiling_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,ceiling_pixels,1);
  freeTexturePixels(ceiling_pixels);
  free(texture_address);
  //Floor
  texture_address = pathJoin(TEXTURES, "test_floor_normal.png", sizeof("test_floor_normal.png") - 1, &size);
  unsigned char* floor_pixels = getTexturePixels(texture_address,&(textures[1].width),&(textures[1].height),&(textures[1].channels));
  createTextureCtx(&(textures[1]),deviceCtx,commandCtx,floor_pixels,1);
  freeTexturePixels(floor_pixels);
  free(texture_address);
  //RWall
  texture_address = pathJoin(TEXTURES, "test_wall_normal.png", sizeof("test_wall_normal.png") - 1, &size);
  unsigned char* rwall_pixels = getTexturePixels(texture_address,&(textures[2].width),&(textures[2].height),&(textures[2].channels));
  createTextureCtx(&(textures[2]),deviceCtx,commandCtx,rwall_pixels,1);
  freeTexturePixels(rwall_pixels);
  free(texture_address);
  //LWall
  texture_address = pathJoin(TEXTURES, "test_wall_normal.png", sizeof("test_wall_normal.png") - 1, &size);
  unsigned char* lwall_pixels = getTexturePixels(texture_address,&(textures[3].width),&(textures[3].height),&(textures[3].channels));
  createTextureCtx(&(textures[3]),deviceCtx,commandCtx,lwall_pixels,1);
  freeTexturePixels(lwall_pixels);
  free(texture_address);
  //FWall
  texture_address = pathJoin(TEXTURES, "test_wall_normal.png", sizeof("test_wall_normal.png") - 1, &size);
  unsigned char* fwall_pixels = getTexturePixels(texture_address,&(textures[4].width),&(textures[4].height),&(textures[4].channels));
  createTextureCtx(&(textures[4]),deviceCtx,commandCtx,fwall_pixels,1);
  freeTexturePixels(fwall_pixels);
  free(texture_address);

  //Air_Gun
  texture_address = pathJoin(TEXTURES, "Sting_Normal.png", sizeof("Sting_Normal.png") - 1, &size);
  unsigned char* sting_pixels = getTexturePixels(texture_address,&(textures[5].width),&(textures[5].height),&(textures[5].channels));
  createTextureCtx(&(textures[5]),deviceCtx,commandCtx,sting_pixels,1);
  freeTexturePixels(sting_pixels);
  free(texture_address);
}

void createTestParallaxes(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  int size;
  char* texture_address = pathJoin(TEXTURES, "test_ceiling_parallax.png", sizeof("test_ceiling_parallax.png") - 1, &size);
  unsigned char* ceiling_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,ceiling_pixels,1);
  freeTexturePixels(ceiling_pixels);
  free(texture_address);
  //Floor
  texture_address = pathJoin(TEXTURES, "test_floor_parallax.png", sizeof("test_floor_parallax.png") - 1, &size);
  unsigned char* floor_pixels = getTexturePixels(texture_address,&(textures[1].width),&(textures[1].height),&(textures[1].channels));
  createTextureCtx(&(textures[1]),deviceCtx,commandCtx,floor_pixels,1);
  freeTexturePixels(floor_pixels);
  free(texture_address);
  //RWall
  texture_address = pathJoin(TEXTURES, "test_wall_parallax.png", sizeof("test_wall_parallax.png") - 1, &size);
  unsigned char* rwall_pixels = getTexturePixels(texture_address,&(textures[2].width),&(textures[2].height),&(textures[2].channels));
  createTextureCtx(&(textures[2]),deviceCtx,commandCtx,rwall_pixels,1);
  freeTexturePixels(rwall_pixels);
  free(texture_address);
  //LWall
  texture_address = pathJoin(TEXTURES, "test_wall_parallax.png", sizeof("test_wall_parallax.png") - 1, &size);
  unsigned char* lwall_pixels = getTexturePixels(texture_address,&(textures[3].width),&(textures[3].height),&(textures[3].channels));
  createTextureCtx(&(textures[3]),deviceCtx,commandCtx,lwall_pixels,1);
  freeTexturePixels(lwall_pixels);
  free(texture_address);
  //FWall
  texture_address = pathJoin(TEXTURES, "Sting_Height.png", sizeof("Sting_Height.png") - 1, &size);
  unsigned char* fwall_pixels = getTexturePixels(texture_address,&(textures[4].width),&(textures[4].height),&(textures[4].channels));
  createTextureCtx(&(textures[4]),deviceCtx,commandCtx,fwall_pixels,1);
  freeTexturePixels(fwall_pixels);
  free(texture_address);


}

void createTestMetallics(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  int size;
  char* texture_address = pathJoin(TEXTURES, "Sting_Metallic.png", sizeof("Sting_Metallic.png") - 1, &size);
  unsigned char* metallic_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,metallic_pixels,1);
  freeTexturePixels(metallic_pixels);
  free(texture_address);
}

void createTestRoughnessMaps(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  int size;
  char* texture_address = pathJoin(TEXTURES, "Sting_Roughness.png", sizeof("Sting_Roughness.png") - 1, &size);
  unsigned char* roughness_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,roughness_pixels,1);
  freeTexturePixels(roughness_pixels);
  free(texture_address);
}

void createTestAoMaps(TextureCtx* textures,int textureCount,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx){
  int size;
  char* texture_address = pathJoin(TEXTURES, "Sting_Mixed_AO.png", sizeof("Sting_Mixed_AO.png") - 1, &size);
  unsigned char* roughness_pixels = getTexturePixels(texture_address,&(textures[0].width),&(textures[0].height),&(textures[0].channels));
  createTextureCtx(&(textures[0]),deviceCtx,commandCtx,roughness_pixels,1);
  freeTexturePixels(roughness_pixels);
  free(texture_address);
}

void createTextureCtx(TextureCtx* texture,VkDeviceCtx* deviceCtx,CommandCtx* commandCtx,unsigned char* pixels,int normal){

  createTextureImage(texture,deviceCtx,texture->width,texture->height,normal);
  VkMemoryRequirements memRequirements;

  vkGetImageMemoryRequirements(
			       deviceCtx->device,
			       texture->textureImage,
			       &memRequirements
			       );
  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex =
    findMemoryType(
		   deviceCtx,
		   memRequirements.memoryTypeBits,
		   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		   );

  if (vkAllocateMemory(
		       deviceCtx->device,
		       &allocInfo,
		       NULL,
		       &texture->textureImageMemory) != VK_SUCCESS) {
    fprintf(stderr, "failed to allocate depth image memory!\n");
    exit(EXIT_FAILURE);
  }

  if (vkBindImageMemory(
			deviceCtx->device,
			texture->textureImage,
			texture->textureImageMemory,
			0) != VK_SUCCESS) {
    fprintf(stderr, "failed to bind depth image memory!\n");
    exit(EXIT_FAILURE);
  }

  //TODO: now copy the pixels
  createTextureBuffer(texture,deviceCtx,commandCtx,pixels);

  VkImageViewCreateInfo createInfo = {0};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = 	texture->textureImage;
  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format =  normal ? VK_FORMAT_R8G8B8A8_UNORM : VK_FORMAT_R8G8B8A8_SRGB;


  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

  if(vkCreateImageView(deviceCtx->device,&createInfo,NULL,&texture->textureImageView)!=VK_SUCCESS){
    fprintf(stderr,"failed to create image views! \n");
    exit(EXIT_FAILURE);
  }

  //free(pixels);
}

void createTextureSampler(VkSampler* sampler, VkDeviceCtx* deviceCtx){
  VkPhysicalDeviceProperties properties = {0};
  vkGetPhysicalDeviceProperties(deviceCtx->physicalDevice, &properties);

  VkSamplerCreateInfo samplerInfo = {0};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;

  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  samplerInfo.anisotropyEnable = VK_TRUE;
  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;

  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f; // raise this if/when you add mipmaps

  if(vkCreateSampler(deviceCtx->device, &samplerInfo, NULL, sampler) != VK_SUCCESS){
    fprintf(stderr, "failed to create texture sampler!\n");
    exit(EXIT_FAILURE);
  }
}


void cleanUpTextures(TextureCtx* textures,uint32_t textureCount,VkDeviceCtx* deviceCtx){
  for(uint32_t i=0;i<textureCount;i++){
    vkDestroyImageView(deviceCtx->device, textures[i].textureImageView, NULL);
    vkDestroyImage(deviceCtx->device, textures[i].textureImage, NULL);
    vkFreeMemory(deviceCtx->device, textures[i].textureImageMemory, NULL);
  }
}


void cleanUpTexturesSampler(VkDeviceCtx* deviceCtx,VkSampler textureSampler){
  vkDestroySampler(deviceCtx->device,textureSampler,NULL);
}
