#pragma once
#include "vk_types.h"
#include "vk_device.h"
#include "../platform/window.h"

typedef struct{
  VkImage colorImage;
  VkImageView colorImageView;
  VkDeviceMemory colorImageMemory;
  VkFormat format;
}MsaaColorCtx;

/*typedef struct{
  VkImage shadowImage;
  VkImageView shadowImageView;
  VkDeviceMemory shadowImageMemory;
  VkFormat format;
  VkSampler shadowSampler;
  }ShadowImageCtx;*/

typedef struct{
  VkImage* shadowImages;
  VkImageView* shadowImageViews;
  VkDeviceMemory* shadowImagesMemory;
  VkFormat format;
  VkSampler shadowSampler;
  uint32_t count;
}ShadowImageCtx;



typedef struct{
  VkSwapchainKHR swapChain;
  VkImage* swapChainImages;
  VkImageView* swapChainImageViews;
  uint32_t imageCount;
  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;
  VkFormat depthImageFormat;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  MsaaColorCtx msaaCtx;
  ShadowImageCtx shadowCtx;
}VkSwapchainCtx;


void createSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface,uint32_t shadowLightsCount);
void cleanupSwapChainCtx(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,uint32_t shadowCount);
void cleanupSwapChainCtxResize(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx);

void createSwapChainCtxResize(VkSwapchainCtx* swapChainCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface);
