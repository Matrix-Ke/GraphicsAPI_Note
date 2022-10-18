#include "ConfigManager.h"

#define USE_STL_TYPE_TRAIT
#define USE_STL_TYPE_TRAIT
#include "Platform/File.h"
#include "Container/Array.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <string>
#include <exception>
#include <stdexcept>


namespace Matrix
{
	void ConfigManager::initialize()
	{
		using namespace rapidjson;
		Platform::MFile   vertexFile(_T("D:/GitDemo/GraphicsAPI_Note/Source/Vulkan/VulkanConfig.Json"));
		unsigned int size = vertexFile.GetFileSize();
		Container::MArray<char>    Buffer;
		Buffer.SetBufferNum(size);
		vertexFile.Read(Buffer.GetBuffer(), sizeof(char), size);
		mEngineConfig.Parse(Buffer.GetBuffer());

		//std::string   name = mEngineConfig["Project_Dir"].GetString();
		if (!mEngineConfig.IsObject())
		{
			std::runtime_error("vulkan config parse fail!");
		}
	}
}
