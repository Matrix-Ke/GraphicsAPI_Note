/*
在通过一个VkInstance来实例化Vulkan库之后，我们在系统中需要寻找并选择一个支持我们所需功能的显卡。
实际上，我们可以选择多个显卡并且同时使用它们，不过在此教程中我们将选择支持我们所需功能的所有显卡中的第一个

家族队列：
1. Vulkan中几乎所有的操作，从绘图上传纹理，都需要将命令提交到队列. 例如，可能有只接受处理计算命令的队列家族，也可能有只接受有关内存传递命令的命令家族。
*/


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstring>
#include <optional>

#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif


class HelloTriangleApplication
{
public:
	void run();

public:
	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		bool isComplete()
		{
			return graphicsFamily.has_value();
		}
	};

private:
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

private:

	void initWindow();
	void initVulkan();
	void createInstance();
	void mainLoop();
	void cleanup();
	void setupDebugMessenger();
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device); //基本设备可用性检查(显卡的功能来检查它们是否支持我们想要的操作，因为不是所有显卡都是相同的)

	//用vkEnumerateInstanceLayerProperties函数列出所有可用的层
	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

private:
	GLFWwindow* window;
	VkInstance  instance;
	VkDebugUtilsMessengerEXT debugMessenger; //Vulkan中的调试回调函数也由一个需要显式创建和销毁的句柄来管理,这种回调函数被称为“messenger”，
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
};


