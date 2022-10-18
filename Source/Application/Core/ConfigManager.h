#pragma once
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


#include <filesystem>

namespace Matrix
{
	struct EngineInitParams;

	class ConfigManager
	{
	public:
		void initialize(const std::filesystem::path& config_file_path);

		const std::filesystem::path& getRootFolder() const;
		const std::filesystem::path& getAssetFolder() const;
		const std::filesystem::path& getShaderSpvFolder() const;

	private:
		std::filesystem::path m_root_folder;
		std::filesystem::path m_asset_folder;
		std::filesystem::path mShaderSpvFolder;

	private:
		rapidjson::Document EditorConfig;
	};
} 
