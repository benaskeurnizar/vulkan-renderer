#include "imgui_bridge.h"
#include <stdio.h>
#include <stdlib.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "IconsFontAwesome7.h"

static void check_vk_result(VkResult err)
{
  if (err == VK_SUCCESS)
    return;

  fprintf(stderr, "[Vulkan] Error: VkResult = %d\n", err);

  if (err < 0)
    exit(EXIT_FAILURE);
}

void imgui_apply_style()
{
  ImGuiStyle& style = ImGui::GetStyle();

  // ------------------------------------------------------------
  // Geometry
  // ------------------------------------------------------------

  style.WindowRounding    = 7.0f;
  style.ChildRounding     = 6.0f;
  style.FrameRounding     = 5.0f;
  style.PopupRounding     = 6.0f;
  style.ScrollbarRounding = 5.0f;
  style.GrabRounding      = 5.0f;

  style.WindowBorderSize  = 1.0f;
  style.ChildBorderSize   = 1.0f;
  style.PopupBorderSize   = 1.0f;
  style.FrameBorderSize   = 0.0f;

  style.FramePadding = ImVec2(12.0f, 6.0f);
  style.ItemSpacing  = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);

  style.WindowPadding = ImVec2(12.0f, 10.0f);

  style.ScrollbarSize = 12.0f;
  style.GrabMinSize   = 10.0f;

  style.SeparatorTextBorderSize = 1.0f;

  // Slightly larger UI
  ImGui::GetIO().FontGlobalScale = 1.05f;


  // ------------------------------------------------------------
  // Colors
  // ------------------------------------------------------------

  ImVec4* colors = style.Colors;

  // Main background
  colors[ImGuiCol_WindowBg] =
    ImVec4(0.055f, 0.065f, 0.085f, 1.00f);

  colors[ImGuiCol_ChildBg] =
    ImVec4(0.060f, 0.070f, 0.090f, 1.00f);

  colors[ImGuiCol_PopupBg] =
    ImVec4(0.075f, 0.085f, 0.105f, 0.98f);

  // Top menu bar
  colors[ImGuiCol_MenuBarBg] =
    ImVec4(0.085f, 0.095f, 0.120f, 1.00f);

  // Borders
  colors[ImGuiCol_Border] =
    ImVec4(0.20f, 0.25f, 0.30f, 0.65f);

  colors[ImGuiCol_BorderShadow] =
    ImVec4(0.0f, 0.0f, 0.0f, 0.20f);


  // ------------------------------------------------------------
  // Text
  // ------------------------------------------------------------

  colors[ImGuiCol_Text] =
    ImVec4(0.91f, 0.93f, 0.95f, 1.00f);

  colors[ImGuiCol_TextDisabled] =
    ImVec4(0.45f, 0.49f, 0.54f, 1.00f);


  // ------------------------------------------------------------
  // Menu interaction
  // ------------------------------------------------------------

  colors[ImGuiCol_Header] =
    ImVec4(0.11f, 0.13f, 0.16f, 1.00f);

  colors[ImGuiCol_HeaderHovered] =
    ImVec4(0.08f, 0.43f, 0.50f, 0.65f);

  colors[ImGuiCol_HeaderActive] =
    ImVec4(0.06f, 0.55f, 0.63f, 0.85f);


  // ------------------------------------------------------------
  // Buttons
  // ------------------------------------------------------------

  colors[ImGuiCol_Button] =
    ImVec4(0.10f, 0.12f, 0.15f, 1.00f);

  colors[ImGuiCol_ButtonHovered] =
    ImVec4(0.10f, 0.30f, 0.34f, 1.00f);

  colors[ImGuiCol_ButtonActive] =
    ImVec4(0.06f, 0.55f, 0.63f, 1.00f);


  // ------------------------------------------------------------
  // Separators
  // ------------------------------------------------------------

  colors[ImGuiCol_Separator] =
    ImVec4(0.25f, 0.32f, 0.36f, 0.55f);

  colors[ImGuiCol_SeparatorHovered] =
    ImVec4(0.10f, 0.55f, 0.63f, 0.75f);

  colors[ImGuiCol_SeparatorActive] =
    ImVec4(0.10f, 0.65f, 0.73f, 1.00f);


  // ------------------------------------------------------------
  // Scrollbars
  // ------------------------------------------------------------

  colors[ImGuiCol_ScrollbarBg] =
    ImVec4(0.04f, 0.05f, 0.065f, 1.00f);

  colors[ImGuiCol_ScrollbarGrab] =
    ImVec4(0.18f, 0.21f, 0.25f, 1.00f);

  colors[ImGuiCol_ScrollbarGrabHovered] =
    ImVec4(0.25f, 0.30f, 0.34f, 1.00f);

  colors[ImGuiCol_ScrollbarGrabActive] =
    ImVec4(0.10f, 0.55f, 0.63f, 1.00f);
}

void imgui_init(Renderer* renderer, Window* window)
{
    QueueFamilyIndices indices =
        findQueueFamilies(renderer->surface, renderer->deviceCtx.physicalDevice);

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Inter_18pt-Regular.ttf", 16.0f);
    ImGui::StyleColorsDark();
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 16.0f;
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Font Awesome 7 Free-Solid-900.otf", 16.0f, &config, icon_ranges);
    
    imgui_apply_style();

    ImGui_ImplGlfw_InitForVulkan(window->window, true);

    ImGui_ImplVulkan_InitInfo init_info = {};

    init_info.Instance =
        renderer->instanceCtx.instance;

    init_info.PhysicalDevice =
        renderer->deviceCtx.physicalDevice;

    init_info.Device =
        renderer->deviceCtx.device;

    init_info.QueueFamily =
        indices.graphicsFamily;

    init_info.Queue =
        renderer->deviceCtx.graphicsQueue;

    init_info.PipelineCache =
        VK_NULL_HANDLE;

    init_info.DescriptorPool =
        renderer->descriptorsCtx.imguiPool;

    init_info.Allocator =
        NULL;

    init_info.MinImageCount =
        renderer->swapChainCtx.imageCount;

    init_info.ImageCount =
        renderer->swapChainCtx.imageCount;

    init_info.PipelineInfoMain.RenderPass =
        renderer->pipelineCtx.renderPass;

    init_info.PipelineInfoMain.Subpass =
        0;

    init_info.PipelineInfoMain.MSAASamples =
        VK_SAMPLE_COUNT_4_BIT;

    init_info.CheckVkResultFn =
        check_vk_result;

    ImGui_ImplVulkan_Init(&init_info);

    // One-time font upload
    //ImGui_ImplVulkan_CreateFontsTexture();
}

void imgui_destroy(){
  ImGui_ImplVulkan_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

  
void imgui_new_frame(){
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();   // <- was just NewFrame()
}

void imgui_draw(Renderer* renderer,World* world)
{
  if (ImGui::BeginMainMenuBar())
    {
      // --- File menu ---
      if (ImGui::BeginMenu("File"))
        {
	  if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN "  Open", "Ctrl+O")) {
	  }
	  if (ImGui::MenuItem(ICON_FA_FLOPPY_DISK "  Save", "Ctrl+S")) {
	  }
	  ImGui::Separator();
	  if (ImGui::MenuItem(ICON_FA_XMARK "  Close", "Alt+F4")) {
	  }
	  ImGui::EndMenu();
        }

      ImGui::SameLine(0.0f, 20.0f);
      ImGui::Separator();
      ImGui::SameLine(0.0f, 20.0f);

      // --- Tool icons ---
      if (ImGui::Button(ICON_FA_ARROWS_ROTATE)) {
	world->render_state ^= GIZMO_ROTATE;
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_HAND)) {
	world->render_state ^= PAN;
      }
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_UP_RIGHT_AND_DOWN_LEFT_FROM_CENTER)) { } // Frame/Zoom to fit
      ImGui::SameLine();
      if (ImGui::Button(ICON_FA_EXPAND)) {
      }
      ImGui::EndMainMenuBar();
    }
}


void imgui_render(VkCommandBuffer command_buffer)
{
    ImGui::Render();   // <- was just Render()
    ImGui_ImplVulkan_RenderDrawData(
        ImGui::GetDrawData(),   // <- was just GetDrawData()
        command_buffer
    );
}
