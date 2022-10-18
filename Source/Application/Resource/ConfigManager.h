#pragma once

#include <rapidjson/document.h>

namespace Matrix
{
	class ConfigManager
	{
	public:
		ConfigManager();
		~ConfigManager();

	public:
		void initialize();

	public:
		rapidjson::Document mEngineConfig;
	};
}