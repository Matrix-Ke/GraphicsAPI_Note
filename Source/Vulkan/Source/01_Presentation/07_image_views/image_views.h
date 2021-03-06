/*
要在渲染管线中使用VkImage，包括交换链中的，我们就需要创建一个VkImageView对象。
一个图像视图（image view）实际上就是一个图像的一个视图。它描述了如何访问图像，以及访问图像的哪一部分，例如这个图像应该被视为一个2D深度纹理，并且不创建多级渐远纹理。
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
	void createImageViews();

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
	std::vector<VkImageView> swapChainImageViews;

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


