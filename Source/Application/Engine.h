#pragma once
#include "Core/GlobalContext.h"

#include <atomic>
#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_set>




namespace Matrix
{
	RuntimeGlobalContext gRuntimeGlobalContext;

	class MatrixEngine
	{

		static const float s_fps_alpha;
	public:
		void startEngine(const std::string& config_file_path);
		void shutdownEngine();

		void initialize();
		void clear();

		bool isQuit() const { return m_is_quit; }
		void run();
		bool tickOneFrame(float delta_time);

		int getFPS() const { return m_fps; }

		/**
		 *  Each frame can only be called once
		 */
		float calculateDeltaTime();

	protected:
		void logicalTick(float delta_time);
		bool rendererTick();

		void calculateFPS(float delta_time);



	protected:
		bool m_is_quit{ false };

		std::chrono::steady_clock::time_point m_last_tick_time_point{ std::chrono::steady_clock::now() };

		float m_average_duration{ 0.f };
		int   m_frame_count{ 0 };
		int   m_fps{ 0 };
	};
}
