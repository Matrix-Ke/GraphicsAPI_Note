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
	public:
		void startEngine(const std::string& config_file_path);
		void shutdownEngine();

		void initialize();
		void clear();

		bool isQuit() const { return mbQuit; }
		void run();
		bool tick(float delta_time);

	private:
		float calculateDeltaTime();

	private:
		bool mbQuit{ false };
	};
}
