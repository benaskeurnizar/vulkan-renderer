
#include "draw.h"
#include "imgui_bridge.h"


void recreateSwapChain(VkSwapchainCtx* swapChainCtx,VkPipelineCtx* pipelineCtx,VkDeviceCtx* deviceCtx,Window* window,VkSurfaceKHR surface){
  int width = 0,height = 0;
  glfwGetFramebufferSize(window->window,&width,&height);
  while(width==0 || height==0){
    glfwGetFramebufferSize(window->window,&width,&height);
    glfwWaitEvents();
  }
  vkDeviceWaitIdle(deviceCtx->device);
  cleanupSwapChainCtxResize(swapChainCtx,deviceCtx);
  //cleanupSwapChainCtx(swapChainCtx,deviceCtx);
  for(uint32_t i=0;i<pipelineCtx->frameBufferCount;i++){
    vkDestroyFramebuffer(deviceCtx->device,pipelineCtx->frameBuffers[i],NULL);
  }
  //vkDestroyFramebuffer(deviceCtx->device,pipelineCtx->shadowFramebuffer,NULL);
  createSwapChainCtxResize(swapChainCtx,deviceCtx,window,surface);
  createFramebuffers(pipelineCtx,swapChainCtx,deviceCtx);
  //createShadowFrameBuffer(pipelineCtx,swapChainCtx,deviceCtx);
}


void updateUniformBuffer(World* world,FrameUniforms* globalUniform,FrameUniforms* meshUniforms,uint32_t currentImage){
  //TODO: map the global one
  UniformBufferGlobal ubog = {0};
  glm_mat4_copy(world->camera.view,ubog.view);
  glm_mat4_copy(world->proj,ubog.proj);
  glm_vec_copy(world->camera.pos,ubog.cam_pos);
  memcpy(ubog.lightSources,world->lightSources, sizeof(LightSource) * world->lightCount);
  ubog.lightCount = world->lightCount;

  memcpy(globalUniform->uniformBuffersMapped[currentImage], &ubog, sizeof(ubog));
  uint32_t meshBuffersIndex = 0;
  for(uint32_t i=0;i<world->meshPbrCount;i++){
    UniformBufferMesh ubom = {0};
    glm_mat4_copy(world->meshesPbr[i].model, ubom.model);
    ubom.has_texture = world->meshesPbr[i].has_texture;
    ubom.has_roughness = world->meshesPbr[i].has_roughness;
    ubom.has_metallic = world->meshesPbr[i].has_metallic;
    ubom.has_ao = world->meshesPbr[i].has_ao;
    ubom.has_normalMap = world->meshesPbr[i].has_normalMap;
    ubom.has_parallaxMap = world->meshesPbr[i].has_parallaxMap;
    ubom.is_pbr = world->meshesPbr[i].is_pbr;
    memcpy(meshUniforms[meshBuffersIndex].uniformBuffersMapped[currentImage],&ubom, sizeof(ubom));
    meshBuffersIndex++;
  }

  for(uint32_t i=0;i< world->meshCount;i++){
    UniformBufferMesh ubom = {0};
    glm_mat4_copy(world->meshes[i].model, ubom.model);
    ubom.has_texture = world->meshes[i].has_texture;
    ubom.has_roughness = world->meshes[i].has_roughness;
    ubom.has_metallic = world->meshes[i].has_metallic;
    ubom.has_ao = world->meshes[i].has_ao;
    ubom.has_normalMap = world->meshes[i].has_normalMap;
     ubom.has_parallaxMap = world->meshes[i].has_parallaxMap;
    ubom.is_pbr = world->meshes[i].is_pbr;
    memcpy(meshUniforms[meshBuffersIndex].uniformBuffersMapped[currentImage],&ubom, sizeof(ubom));
    meshBuffersIndex++;
  }

}

void updateLightUniform(World* world,FrameUniforms* lightUniform,FrameUniforms* meshUniforms,uint32_t currentImage){
  uint32_t shadowLightsCount = 0;
  for(uint32_t i=0;i<world->lightCount;i++){
    if(world->lightSources[i].has_shadow){
      shadowLightsCount++;
    }
  }
  uint32_t shadowIndex = 0;

  for(uint32_t i = 0; i < world->lightCount; i++){
    if(world->lightSources[i].has_shadow){
      UniformBufferLight ubog = {0};

      glm_mat4_copy(
		    world->lightSources[i].viewProj,
		    ubog.lightViewProj
		    );

      memcpy(
	     lightUniform[shadowIndex].uniformBuffersMapped[currentImage],
	     &ubog,
	     sizeof(ubog)
	     );

      shadowIndex++;
    }
  }
 
  uint32_t meshBuffersIndex = 0;
  for(uint32_t i=0;i<world->meshPbrCount;i++){
    UniformBufferMesh ubom = {0};
    glm_mat4_copy(world->meshesPbr[i].model, ubom.model);
    ubom.has_texture = world->meshesPbr[i].has_texture;
    ubom.has_roughness = world->meshesPbr[i].has_roughness;
    ubom.has_metallic = world->meshesPbr[i].has_metallic;
    ubom.has_ao = world->meshesPbr[i].has_ao;
    ubom.has_normalMap = world->meshesPbr[i].has_normalMap;
    ubom.has_parallaxMap = world->meshesPbr[i].has_parallaxMap;
    ubom.is_pbr = world->meshesPbr[i].is_pbr;
    memcpy(meshUniforms[meshBuffersIndex].uniformBuffersMapped[currentImage],&ubom, sizeof(ubom));
    meshBuffersIndex++;
  }

  for(uint32_t i=0;i< world->meshCount;i++){
    UniformBufferMesh ubom = {0};
    glm_mat4_copy(world->meshes[i].model, ubom.model);
    ubom.has_texture = world->meshes[i].has_texture;
    ubom.has_roughness = world->meshes[i].has_roughness;
    ubom.has_metallic = world->meshes[i].has_metallic;
    ubom.has_ao = world->meshes[i].has_ao;
    ubom.has_normalMap = world->meshes[i].has_normalMap;
    ubom.has_parallaxMap = world->meshes[i].has_parallaxMap;
    ubom.is_pbr = world->meshes[i].is_pbr;
    memcpy(meshUniforms[meshBuffersIndex].uniformBuffersMapped[currentImage],&ubom, sizeof(ubom));
    meshBuffersIndex++;
  }
}

void shadowPass(Renderer* renderer,World* world,VkCommandBuffer commandBuffer){
  VkRenderPassBeginInfo shadowBeginInfo = {0};
  shadowBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  shadowBeginInfo.renderPass = renderer->pipelineCtx.shadowRenderPass;
  shadowBeginInfo.renderArea.offset = (VkOffset2D){0, 0};
  shadowBeginInfo.renderArea.extent = (VkExtent2D){2048, 2048};

  VkClearValue clearDepth;
  clearDepth.depthStencil.depth = 1.0;
  clearDepth.depthStencil.stencil = 0;

  shadowBeginInfo.clearValueCount = 1;
  shadowBeginInfo.pClearValues = &clearDepth;

  

  VkViewport shadowViewport = {0,0, 2048,2048, 0.0f,1.0f};
  VkRect2D shadowScissor = {{0,0},{2048,2048}};
  vkCmdSetViewport(commandBuffer, 0, 1, &shadowViewport);
  vkCmdSetScissor(commandBuffer, 0, 1, &shadowScissor);

  uint32_t shadowLightsCount = 0;
  for(uint32_t i=0;i<world->lightCount;i++){
    if(world->lightSources[i].has_shadow){
      shadowLightsCount++;
    }
  }
  uint32_t totalMeshCount = world->meshCount+world->meshPbrCount;
  uint32_t meshBufferIndex = 0;
  for(uint32_t j=0;j<shadowLightsCount;j++){
    meshBufferIndex = 0;
    shadowBeginInfo.framebuffer = renderer->pipelineCtx.shadowFramebuffers[j];
    vkCmdBeginRenderPass(commandBuffer,
			 &shadowBeginInfo,
			 VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer,
		      VK_PIPELINE_BIND_POINT_GRAPHICS,
		      renderer->pipelineCtx.shadowPipeline);
    for (uint32_t i = 0; i < world->meshPbrCount; i++) {

      VkBuffer vertexBuffers[] = {
	renderer->meshBuffers[meshBufferIndex].vertexBuffer
      };
      VkDeviceSize offsets[] = {0};

      vkCmdBindVertexBuffers(commandBuffer,
			     0,
			     1,
			     vertexBuffers,
			     offsets);

      vkCmdBindIndexBuffer(commandBuffer,
			   renderer->meshBuffers[meshBufferIndex].indexBuffer,
			   0,
			   VK_INDEX_TYPE_UINT32);

      vkCmdBindDescriptorSets(
			      commandBuffer,
			      VK_PIPELINE_BIND_POINT_GRAPHICS,
			      renderer->pipelineCtx.shadowPipelineLayout,
			      0,
			      1,
			      &renderer->descriptorsCtx.shadowSets[
								   (j*totalMeshCount+i) * MAX_FRAMES_IN_FLIGHT + renderer->currentFrame
								   ],
			      0,
			      NULL);

      vkCmdDrawIndexed(commandBuffer,
		       world->meshesPbr[i].indexCount,
		       1,
		       0,
		       0,
		       0);
      meshBufferIndex++;
    }
    
    for (uint32_t i = 0; i < world->meshCount; i++) {

      VkBuffer vertexBuffers[] = {
	renderer->meshBuffers[meshBufferIndex].vertexBuffer
      };
      VkDeviceSize offsets[] = {0};

      vkCmdBindVertexBuffers(commandBuffer,
			     0,
			     1,
			     vertexBuffers,
			     offsets);

      vkCmdBindIndexBuffer(commandBuffer,
			   renderer->meshBuffers[meshBufferIndex].indexBuffer,
			   0,
			   VK_INDEX_TYPE_UINT32);

      vkCmdBindDescriptorSets(
			      commandBuffer,
			      VK_PIPELINE_BIND_POINT_GRAPHICS,
			      renderer->pipelineCtx.shadowPipelineLayout,
			      0,
			      1,
			      &renderer->descriptorsCtx.shadowSets[
								   ((j*totalMeshCount)+i+world->meshPbrCount) * MAX_FRAMES_IN_FLIGHT + renderer->currentFrame
								   ],
			      0,
			      NULL);

      vkCmdDrawIndexed(commandBuffer,
		       world->meshes[i].indexCount,
		       1,
		       0,
		       0,
		       0);
      meshBufferIndex++;
    }
    vkCmdEndRenderPass(commandBuffer);
  }
}

void recordCommandBuffer(Renderer* renderer,
                         World* world,
                         uint32_t imageIndex)
{
  VkCommandBuffer commandBuffer =
    renderer->commandsCtx.buffers[renderer->currentFrame];

  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
    fprintf(stderr, "failed to begin recording command buffer!\n");
    exit(EXIT_FAILURE);
  }

  shadowPass(renderer,world,commandBuffer);

  VkRenderPassBeginInfo renderPassInfo = {0};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = renderer->pipelineCtx.renderPass;
  renderPassInfo.framebuffer = renderer->pipelineCtx.frameBuffers[imageIndex];
  renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
  renderPassInfo.renderArea.extent = renderer->swapChainCtx.swapChainExtent;

  VkClearValue clearValues[3] = {0};
  clearValues[0].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  clearValues[1].depthStencil.depth = 1.0f;
  clearValues[1].depthStencil.stencil = 0;
  clearValues[2].color = (VkClearColorValue){{0.0f, 0.0f, 0.0f, 1.0f}};
  
  renderPassInfo.clearValueCount = 3;
  renderPassInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(commandBuffer,
		       &renderPassInfo,
		       VK_SUBPASS_CONTENTS_INLINE);

  

  VkViewport viewport = {0};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)renderer->swapChainCtx.swapChainExtent.width;
  viewport.height = (float)renderer->swapChainCtx.swapChainExtent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

  VkRect2D scissor = {0};
  scissor.offset = (VkOffset2D){0, 0};
  scissor.extent = renderer->swapChainCtx.swapChainExtent;
  vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  
  uint32_t meshBuffersIndex = 0;
  //NOTE: draw the meshes
  for(uint32_t i=0; i< world->meshPbrCount;i++){
    vkCmdBindPipeline(commandBuffer,
		      VK_PIPELINE_BIND_POINT_GRAPHICS,
		      renderer->pipelineCtx.trianglePipelinePBR);
    VkBuffer vertexBuffers[] = {
      renderer->meshBuffers[meshBuffersIndex].vertexBuffer
    };

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer,
			   0,
			   1,
			   vertexBuffers,
			   offsets);

    vkCmdBindIndexBuffer(commandBuffer,
			 renderer->meshBuffers[meshBuffersIndex].indexBuffer,
			 0,
			 VK_INDEX_TYPE_UINT32);

    vkCmdBindDescriptorSets(
			    commandBuffer,
			    VK_PIPELINE_BIND_POINT_GRAPHICS,
			    renderer->pipelineCtx.pipelineLayoutPbr,
			    0,
			    1,
			    &renderer->descriptorsCtx.pbrSets[
							      i * MAX_FRAMES_IN_FLIGHT + renderer->currentFrame
							      ],
			    0,
			    NULL);
    vkCmdDrawIndexed(commandBuffer,
		     world->meshesPbr[i].indexCount,
		     1,
		     0,
		     0,
		     0);
    meshBuffersIndex++;
  }
  
  for (uint32_t i = 0; i < world->meshCount; i++) {
    vkCmdBindPipeline(commandBuffer,
		      VK_PIPELINE_BIND_POINT_GRAPHICS,
		      renderer->pipelineCtx.trianglePipeline);
   
    VkBuffer vertexBuffers[] = {
      renderer->meshBuffers[meshBuffersIndex].vertexBuffer
    };
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer,
			   0,
			   1,
			   vertexBuffers,
			   offsets);

    vkCmdBindIndexBuffer(commandBuffer,
			 renderer->meshBuffers[meshBuffersIndex].indexBuffer,
			 0,
			 VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(
			    commandBuffer,
			    VK_PIPELINE_BIND_POINT_GRAPHICS,
			    renderer->pipelineCtx.pipelineLayout,
			    0,
			    1,
			    &renderer->descriptorsCtx.sets[
							   (i) * MAX_FRAMES_IN_FLIGHT + renderer->currentFrame
							   ],
			    0,
			    NULL);

    vkCmdDrawIndexed(commandBuffer,
		     world->meshes[i].indexCount,
		     1,
		     0,
		     0,
		     0);
    meshBuffersIndex++;
  }
    
  vkCmdEndRenderPass(commandBuffer);

  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
    fprintf(stderr, "failed to record command buffer!\n");
    exit(EXIT_FAILURE);
  }
}

void drawFrame(Renderer* renderer,Window* window,World* world){
  vkWaitForFences(renderer->deviceCtx.device, 1, &(renderer->syncCtx.in_flight[renderer->currentFrame]), VK_TRUE, UINT64_MAX);

  uint32_t imageIndex;
  VkResult result = vkAcquireNextImageKHR(renderer->deviceCtx.device,renderer->swapChainCtx.swapChain,UINT64_MAX,renderer->syncCtx.image_available[renderer->currentFrame],VK_NULL_HANDLE,&imageIndex);
  if(result == VK_ERROR_OUT_OF_DATE_KHR ){
    recreateSwapChain(&(renderer->swapChainCtx),&(renderer->pipelineCtx),&(renderer->deviceCtx),window,renderer->surface);
    return;
  }else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
    fprintf(stderr,"failed to acquire swap chain image!\n");
    exit(EXIT_FAILURE);
  }
  //Only reset the fence if we are submitting work
  vkResetFences(renderer->deviceCtx.device, 1, &(renderer->syncCtx.in_flight[renderer->currentFrame]));
  updateLightUniform(world,(renderer->lightUniforms),renderer->meshUniforms,renderer->currentFrame);
  updateUniformBuffer(world,&(renderer->globalUniform),renderer->meshUniforms,renderer->currentFrame);
 

  vkResetCommandBuffer(renderer->commandsCtx.buffers[renderer->currentFrame],0);
  recordCommandBuffer(renderer,world,imageIndex);
    
  
  //submitting the command buffer
  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  VkSemaphore waitSemaphores[] = {renderer->syncCtx.image_available[renderer->currentFrame]};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;

  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &(renderer->commandsCtx.buffers[renderer->currentFrame]);
  submitInfo.signalSemaphoreCount = 1;
  VkSemaphore signalSemaphores[] = {renderer->syncCtx.render_finished[imageIndex]};
  submitInfo.pSignalSemaphores = signalSemaphores;
  if(vkQueueSubmit(renderer->deviceCtx.graphicsQueue, 1, &submitInfo, renderer->syncCtx.in_flight[renderer->currentFrame]) != VK_SUCCESS){
    fprintf(stderr,"failed to submit draw command buffer!\n");
    exit(EXIT_FAILURE);
  }

  VkPresentInfoKHR presentInfo = {0};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = signalSemaphores;
  VkSwapchainKHR swapChains[] = {renderer->swapChainCtx.swapChain};
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = NULL; // Optional
  result = vkQueuePresentKHR(renderer->deviceCtx.presentQueue, &presentInfo);
  if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderer->framebufferResized || world->resized){
    world->resized = 0;
    renderer->framebufferResized = false;
    recreateSwapChain(&(renderer->swapChainCtx),&(renderer->pipelineCtx),&(renderer->deviceCtx),window,renderer->surface);
  }else if(result != VK_SUCCESS){
    fprintf(stderr,"failed to present swap chain image!\n");
    exit(EXIT_FAILURE);
  }
  
  renderer->currentFrame = (renderer->currentFrame+1)%MAX_FRAMES_IN_FLIGHT;
 
}
