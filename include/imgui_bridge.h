#ifndef IMGUI_BRIDGE_H
#define IMGUI_BRIDGE_H

#include "graphics.h"
#include "renderer.h"
#include "vk_types.h"
#include "vk_device.h"
#include "vk_command.h"
#include "vk_buffer.h"
#include "window.h"
#ifdef __cplusplus
extern "C" {
#endif

  void imgui_init(Renderer* renderer, Window* window);
  void imgui_new_frame(void);
  void imgui_render(VkCommandBuffer commandBuffer);
  void imgui_shutdown(void);
  void imgui_draw(Renderer* renderer,World* world);
  void imgui_destroy();
#ifdef __cplusplus
}
#endif

#endif
