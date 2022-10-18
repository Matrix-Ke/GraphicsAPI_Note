#include "GlobalContext.h"

#include "Core/ConfigManager.h"
#include "render/RenderSystem.h"
#include "Editor/WindowsApplication.h"


namespace Matrix
{
	RuntimeGlobalContext g_runtime_global_context;

	void RuntimeGlobalContext::startSystems(const std::string& config_file_path)
	{
		//todo。。。各个系统的初始化如何顺序控制 ？
		m_logger_system = std::make_shared<LogSystem>();

		m_config_manager = std::make_shared<ConfigManager>();
		m_config_manager->initialize(config_file_path);


		m_window_system = std::make_shared<WindowSystem>();
		WindowCreateInfo window_create_info;
		m_window_system->initialize(window_create_info);

		m_render_system = std::make_shared<RenderSystem>();
		RenderSystemInitInfo render_init_info;
		render_init_info.window_system = m_window_system;
		m_render_system->initialize(render_init_info);

	}
	void RuntimeGlobalContext::shutdownSystems()
	{
		m_render_system.reset();

		m_window_system.reset();

		m_logger_system.reset();

		m_config_manager.reset();

	}
}
