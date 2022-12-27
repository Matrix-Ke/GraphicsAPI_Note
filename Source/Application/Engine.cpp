#include "Engine.h"
#include "Render/RenderSystem.h"

#include "Core/logSysytem.h"

namespace Matrix
{
	void MatrixEngine::startEngine(const std::string& config_file_path)
	{
		//初始化各类引擎子系统 
		g_runtime_global_context.startSystems(config_file_path);

		LOG_INFO("engine start");
	}

	void MatrixEngine::shutdownEngine()
	{
		LOG_INFO("engine shutdown");

		g_runtime_global_context.shutdownSystems();

	}

	void MatrixEngine::initialize() {}
	void MatrixEngine::clear() {}

	void MatrixEngine::run()
	{
		std::shared_ptr<WindowSystem> window_system = g_runtime_global_context.m_window_system;
		assert(window_system);

		while (!window_system->shouldClose())
		{
			const float delta_time = calculateDeltaTime();
			tickOneFrame(delta_time);
		}
	}

	float MatrixEngine::calculateDeltaTime()
	{
		float delta_time;
		{
			using namespace std::chrono;

			steady_clock::time_point tick_time_point = steady_clock::now();
			duration<float> time_span = duration_cast<duration<float>>(tick_time_point - m_last_tick_time_point);
			delta_time = time_span.count();

			m_last_tick_time_point = tick_time_point;
		}
		return delta_time;
	}

	bool MatrixEngine::tickOneFrame(float delta_time)
	{
		logicalTick(delta_time);
		calculateFPS(delta_time);

		// single thread
		// exchange data between logic and render contexts
		g_runtime_global_context.m_render_system->swapLogicRenderData();

		rendererTick();

#ifdef ENABLE_PHYSICS_DEBUG_RENDERER
		g_runtime_global_context.m_physics_manager->renderPhysicsWorld(delta_time);
#endif

		g_runtime_global_context.m_window_system->pollEvents();


		g_runtime_global_context.m_window_system->setTitle(
			std::string("Piccolo - " + std::to_string(getFPS()) + " FPS").c_str());

		const bool should_window_close = g_runtime_global_context.m_window_system->shouldClose();
		return !should_window_close;
	}

	void MatrixEngine::logicalTick(float delta_time)
	{
		//g_runtime_global_context.m_world_manager->tick(delta_time);
		//g_runtime_global_context.m_input_system->tick();
	}

	bool MatrixEngine::rendererTick()
	{
		g_runtime_global_context.m_render_system->tick();
		return true;
	}

	const float MatrixEngine::s_fps_alpha = 1.f / 100;

	void        MatrixEngine::calculateFPS(float delta_time)
	{
		m_frame_count++;

		if (m_frame_count == 1)
		{
			m_average_duration = delta_time;
		}
		else
		{
			m_average_duration = m_average_duration * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
		}

		m_fps = static_cast<int>(1.f / m_average_duration);
	}
}


