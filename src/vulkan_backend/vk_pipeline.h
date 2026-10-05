#pragma once

#include "vk_types.h"
#include "vk_swapChain.h"
#include "vk_device.h"
#include "vk_descriptor.h"
#include "../core/file.h"
#include "../graphics/graphics.h"

typedef struct{
  VkPipeline trianglePipeline;
  VkPipeline trianglePipelinePBR;
  VkPipeline shadowPipeline;
  VkPipelineLayout pipelineLayout;
  VkPipelineLayout shadowPipelineLayout;
  VkPipelineLayout pipelineLayoutPbr;
  VkRenderPass renderPass;
  VkRenderPass shadowRenderPass;
  VkFramebuffer* frameBuffers;      
  uint32_t frameBufferCount;
  VkFramebuffer* shadowFramebuffers;
  uint32_t shadowFrameBufferCount;
}VkPipelineCtx;

typedef struct{
  VkViewport viewport;
  VkRect2D scissor;
  
  VkPipelineViewportStateCreateInfo viewportState;
  VkPipelineDynamicStateCreateInfo dynamicState;
  VkPipelineRasterizationStateCreateInfo rasterizer;
  VkPipelineMultisampleStateCreateInfo multisampling;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlending;
  VkPipelineDepthStencilStateCreateInfo depthStencil;
}pipelineStateDescriptions;


void createPipelineCtx(VkPipelineCtx* pipelineCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,DescriptorCtx* descriptorCtx,uint32_t shadowImagesCount);
void cleanupPipelineCtx(VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx);

void createFramebuffers(VkPipelineCtx* pipelineCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx);

void createShadowFrameBuffers(VkPipelineCtx* pipelineCtx,VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,uint32_t count);
