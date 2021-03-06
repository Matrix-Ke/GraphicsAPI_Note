/*
为了在Vulkan与窗口系统之间建立连接，以让我们把渲染结果显示到屏幕上，我们需要WSI（Window System Interface，窗口系统接口）扩展.
VK_KHR_surface。它暴露一个VkSurfaceKHR对象，这个对象作为表面的一个抽象类型，来显示渲染好的图像（image）
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
		std::optional<uint32_t> graphicsFamily; //绘制
		std::optional<uint32_t> presentFamily;// 显示

		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
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
	void createSurface();

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
	VkDevice device; //逻辑设备
	VkQueue graphicsQueue;//队列随逻辑设备创建而自动创建，设备销毁而自动销毁
	VkSurfaceKHR surface; //表面需要在实例创建之后再创建，因为它可以影响物理设备的选择
	VkQueue presentQueue;
};


