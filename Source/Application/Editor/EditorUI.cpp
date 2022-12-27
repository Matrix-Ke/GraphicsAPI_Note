#include "EditorUI.h"

#include "Editor/Editor.h"
#include "Editor/WindowsApplication.h"
#include "Core/ConfigManager.h"
#include "Core/GlobalContext.h"
#include "Render/RenderSystem.h"

#include "GLFW/glfw3.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <stb_image.h>

namespace Matrix
{
	EditorUI::EditorUI()
	{
	}

	void EditorUI::showEditorUI()
	{
		showEditorMenu(&m_editor_menu_window_open);
	}

	void EditorUI::showEditorMenu(bool* p_open)
	{
		ImGuiDockNodeFlags dock_flags = ImGuiDockNodeFlags_DockSpace;
		ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
			ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
		std::array<int, 2> window_size = g_editor_global_context.m_window_system->getWindowSize();
		ImGui::SetNextWindowSize(ImVec2((float)window_size[0], (float)window_size[1]), ImGuiCond_Always);

		ImGui::SetNextWindowViewport(main_viewport->ID);

		ImGui::Begin("Editor menu", p_open, window_flags);

		ImGui::End();
	}

	void EditorUI::initialize(WindowUIInitInfo init_info)
	{
		std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
		assert(config_manager);

		// create imgui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// set ui content scale
		float x_scale, y_scale;
		glfwGetWindowContentScale(init_info.window_system->getWindow(), &x_scale, &y_scale);
		float content_scale = fmaxf(1.0f, fmaxf(x_scale, y_scale));

		// load font for imgui
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingAlwaysTabBar = true;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.Fonts->AddFontFromFileTTF(config_manager->getEditorFontPath().generic_string().data(), content_scale * 16, nullptr, nullptr);
		io.Fonts->Build();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowPadding = ImVec2(1.0, 0);
		style.FramePadding = ImVec2(14.0, 2.0f);
		style.ChildBorderSize = 0.0f;
		style.FrameRounding = 5.0f;
		style.FrameBorderSize = 1.5f;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		// initialize imgui vulkan render backend
		init_info.render_system->initializeUIRenderBackend(this);
	}

	void EditorUI::preRender() { showEditorUI(); }

}

