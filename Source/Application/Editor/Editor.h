#pragma once

#include "Math/Vector2.h"

#include <memory>

namespace Matrix
{
	class EditorUI;
	class MatrixEngine;


	struct EditorGlobalContextInitInfo
	{
		class WindowSystem* window_system;
		class RenderSystem* render_system;
		class MatrixEngine* engine_runtime;
	};

	class EditorGlobalContext
	{
	public:
		class EditorSceneManager* m_scene_manager{ nullptr };
		class EditorInputManager* m_input_manager{ nullptr };
		class RenderSystem* m_render_system{ nullptr };
		class WindowSystem* m_window_system{ nullptr };
		class MatrixEngine* m_engine_runtime{ nullptr };

	public:
		void initialize(const EditorGlobalContextInitInfo& init_info);
		void clear();
	};

	extern EditorGlobalContext g_editor_global_context;

	class MatrixEditor
	{
	public:
		MatrixEditor();
		virtual ~MatrixEditor();

	public:
		void initialize(MatrixEngine* engine_runtime);
		void clear();
		void run();

	private:
		std::shared_ptr<EditorUI> m_editor_ui;
		MatrixEngine* m_engine_runtime{ nullptr };
	};
}