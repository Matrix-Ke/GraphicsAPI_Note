#pragma once
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include <memory>
#include <chrono>
#include <map>


namespace Matrix
{
	class WindowSystem;
	class RenderSystem;

	struct WindowUIInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
		std::shared_ptr<RenderSystem> render_system;
	};

	class EditorUI
	{
	public:
		EditorUI();

	private:

		void showEditorUI();
		void showEditorMenu(bool* p_open);


	public:
		virtual void initialize(WindowUIInitInfo init_info);
		virtual void preRender();

	private:
		std::unordered_map<std::string, std::function<void(std::string, void*)>> m_editor_ui_creator;
		std::unordered_map<std::string, unsigned int>                            m_new_object_index_map;
		std::chrono::time_point<std::chrono::steady_clock>                       m_last_file_tree_update;

		bool m_editor_menu_window_open = true;
		bool m_asset_window_open = true;
		bool m_game_engine_window_open = true;
		bool m_file_content_window_open = true;
		bool m_detail_window_open = true;
		bool m_scene_lights_window_open = true;
		bool m_scene_lights_data_window_open = true;
	};
}