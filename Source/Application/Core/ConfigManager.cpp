#include "ConfigManager.h"
#include "logSysytem.h"

#include "Platform/File.h"
#include "Container/Array.h"

#include <fstream>
#include <string>


namespace Matrix
{
	void ConfigManager::initialize(const std::filesystem::path& config_file_path)
	{
		using namespace rapidjson;
		Platform::MFile   vertexFile(config_file_path.string().c_str());
		unsigned int size = vertexFile.GetFileSize();
		Container::MArray<char>    Buffer;
		Buffer.SetBufferNum(size);
		vertexFile.Read(Buffer.GetBuffer(), sizeof(char), size);
		EditorConfig.Parse(Buffer.GetBuffer());

		//std::string   name = EditorConfig["Project_Dir"].GetString();
		if (!EditorConfig.IsObject())
		{
			LOG_ERROR("vulkan config parse fail!")
		}
		LOG_INFO("Editor Config:" + config_file_path.string() + "has load!");

		//路径需要检查是否存在
		m_root_folder = EditorConfig["Project_Dir"].GetString();
		m_asset_folder = EditorConfig["Project_Resource_Dir"].GetString();
		mShaderSpvFolder = EditorConfig["Shader_Generated_Dir"].GetString();
	}

	const std::filesystem::path& ConfigManager::getRootFolder() const { return m_root_folder; }

	const std::filesystem::path& ConfigManager::getAssetFolder() const { return m_asset_folder; }

	const std::filesystem::path& ConfigManager::getShaderSpvFolder() const
	{
		return mShaderSpvFolder;
	}

} 
