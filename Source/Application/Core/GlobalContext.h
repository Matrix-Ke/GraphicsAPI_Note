#pragma once
#include "Render/RenderPass.h"


#include <memory>
#include <string>



namespace Matrix
{
	class ConfigManager;
	class RenderSystem;
	class LogSystem;
	class  WindowSystem;
	struct EngineInitParams;

	/// Manage the lifetime and creation/destruction order of all global system
	class RuntimeGlobalContext
	{
	public:
		// create all global systems and initialize these systems
		void startSystems(const std::string& config_file_path);
		// destroy all global systems
		void shutdownSystems();

	public:
		std::shared_ptr<ConfigManager>   m_config_manager;
		std::shared_ptr<LogSystem>       m_logger_system;
		std::shared_ptr<WindowSystem>    m_window_system;
		std::shared_ptr<RenderSystem>    m_render_system;

	};

	extern RuntimeGlobalContext g_runtime_global_context;
}

