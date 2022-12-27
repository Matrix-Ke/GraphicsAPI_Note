#include "Editor.h"

#include "Engine.h"
#include "Core/GlobalContext.h"
#include "Render/RenderSystem.h"
#include "Editor/EditorSceneManager.h"
#include "Editor/EditorInputManager.h"
#include "Editor/EditorUI.h"

#include <string>

namespace Matrix
{
	EditorGlobalContext g_editor_global_context;

	void EditorGlobalContext::initialize(const EditorGlobalContextInitInfo& init_info)
	{
		g_editor_global_context.m_window_system = init_info.window_system;
		g_editor_global_context.m_render_system = init_info.render_system;
		g_editor_global_context.m_engine_runtime = init_info.engine_runtime;

		m_scene_manager = new EditorSceneManager();
		m_input_manager = new EditorInputManager();
		m_scene_manager->initialize();
		m_input_manager->initialize();
	}

	void EditorGlobalContext::clear()
	{
		delete (m_scene_manager);
		delete (m_input_manager);
	}

	//MatrixEditor
	void registerEdtorTickComponent(std::string component_type_name)
	{
	}

	MatrixEditor::MatrixEditor()
	{
	}

	MatrixEditor::~MatrixEditor()
	{
	}

	void MatrixEditor::initialize(MatrixEngine* engine_runtime)
	{
		assert(engine_runtime);

		m_engine_runtime = engine_runtime;

		EditorGlobalContextInitInfo init_info = { g_runtime_global_context.m_window_system.get(),
												 g_runtime_global_context.m_render_system.get(),
												 engine_runtime };
		g_editor_global_context.initialize(init_info);
		g_editor_global_context.m_scene_manager->setEditorCamera(g_runtime_global_context.m_render_system->getRenderCamera());

		//init Editor UI
		m_editor_ui = std::make_shared<EditorUI>();
		WindowUIInitInfo ui_init_info = { g_runtime_global_context.m_window_system, g_runtime_global_context.m_render_system };
		m_editor_ui->initialize(ui_init_info);
	}

	void MatrixEditor::clear() { g_editor_global_context.clear(); }

	void MatrixEditor::run()
	{
		assert(m_engine_runtime);
		assert(m_editor_ui);
		float delta_time;

		while (true)
		{
			delta_time = m_engine_runtime->calculateDeltaTime();
			g_editor_global_context.m_scene_manager->tick(delta_time);
			g_editor_global_context.m_input_manager->tick(delta_time);
			if (!m_engine_runtime->tickOneFrame(delta_time))
				return;
		}
	}
}

