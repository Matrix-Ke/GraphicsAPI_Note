/*
Vulkan中没有“默认帧缓冲”的概念，于是就需要有一个“基础设施”，它拥有这么一个缓冲区，让我们在显示到屏幕上之前把图像渲染在上面。这个“基础设施”就是“交换链”（swap chain）
swap_chain 需要被显示创建（本质就是一个图像队列）,创建交换链需要查询一些信息：(结构体SwapChainSupportDetails记录)
1。surface的基本兼容性（交换链支持的最小最大图像数量， 最小最大宽高）
2. 表面格式 （像素格式，色彩空间）
3. 可用的显示模式 : 显示模式是交换链最重要的设置项，因为它决定了如何把图像显示到屏幕上。Vulkan中有四种可用的显示模式：1. 立即模式 2.队列排对刷新（双缓冲）
				3.不等待上一个，直接将渲染好的而图像显示大屏幕，会撕裂画面 4.不阻塞，直接用新的代替队列中的图像
*/

#pragma once
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
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
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

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};


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
	void createSwapChain();
	bool isDeviceSuitable(VkPhysicalDevice device); //基本设备可用性检查(显卡的功能来检查它们是否支持我们想要的操作，因为不是所有显卡都是相同的)

	//用vkEnumerateInstanceLayerProperties函数列出所有可用的层
	bool checkValidationLayerSupport();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	//仅仅是查询交换链是否可用还不够，因为它有可能与我们的表面不兼容。创建交换链同样涉及到许多设置项目，而且比创建实例和设备的设置项目多得多
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	std::vector<const char*> getRequiredExtensions();
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device); /*findQueueFamilies函数来寻找需要的所有队列家族, 例如显示，图形等*/
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator);

private:
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

private:
	GLFWwindow* window;
	VkInstance  instance;
	VkDebugUtilsMessengerEXT debugMessenger; //Vulkan中的调试回调函数也由一个需要显式创建和销毁的句柄来管理,这种回调函数被称为“messenger”，
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device; //逻辑设备
	VkSurfaceKHR surface; //表面需要在实例创建之后再创建，因为它可以影响物理设备的选择
	VkQueue graphicsQueue;//队列随逻辑设备创建而自动创建，设备销毁而自动销毁
	VkQueue presentQueue;

};


