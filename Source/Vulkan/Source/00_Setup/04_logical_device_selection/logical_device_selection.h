/*
选用了一个物理设备之后，我们需要创建一个“逻辑设备”（logical device）作为它的接口。创建逻辑设备的过程与创建实例的过程很相似，并且需要描述我们想要使用的功能。
同样，在查询了有哪些队列家族可用之后，我们需要在创建逻辑设备时指定创建哪些队列。如果你需要的话，甚至可以根据同一个物理设备创建多个逻辑设备。
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
	void createLogicalDevice();

	bool isDeviceSuitable(VkPhysicalDevice device); //基本设备可用性检查(显卡的功能来检查它们是否支持我们想要的操作，因为不是所有显卡都是相同的)

	//用vkEnumerateInstanceLayerProperties函数列出所有可用的层
	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

private:
	GLFWwindow* window;
	VkInstance  instance;
	VkDebugUtilsMessengerEXT debugMessenger; //Vulkan中的调试回调函数也由一个需要显式创建和销毁的句柄来管理,这种回调函数被称为“messenger”，
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue graphicsQueue;//队列随逻辑设备创建而自动创建，设备销毁而自动销毁
};


