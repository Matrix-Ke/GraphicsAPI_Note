/*
* 验证层：
* vulkan是基于最小化驱动负担的思想设计的， 这样在默认情况下错误检查非常有限，很容易导致许多小的问题发生。 因此加上了一层validation来进行错误检查
* 验证层是连接在vulkan上的可选组件：1. 根据规范检测参数值； 2.追踪对象创建析构过程，检测资源是否泄露； 3.线程可以被追踪； 4. 每个调用和参数都记录在标准输出上； 5.可以追踪函数调用（以便进行性能分析）

* 理解vulkan扩展：
* 1. 仅仅启用验证层的话没什么帮助，因为它们现在没有办法把错误信息发送回我们的程序。为了接收这些信息，我们需要设置一个回调函数，这需要VK_EXT_debug_utils插件。（VK_EXT_DEBUG_UTILS_EXTENSION_NAME宏）

*/


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cstring>

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
	void run()
	{
		/*
		* 四部曲：
		1. 创建资源（例如窗口，vkInstance)
		2. 图形API操作
		2. 主循环
		3. 销毁资源（例如窗口，vkInstance,消息等）
		*/
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:

	void initWindow()
	{
		//这个函数初始化GLFW库
		glfwInit();

		//为GLFW原本是为创建OpenGL上下文（context）设计的，所以我们接下来需要调用函数告诉GLFW不要创建OpenGL上下文
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//允许窗口调整大小会产生许多额外的问题，这一点日后再谈，现在先通过调用另一个window hint函数禁用掉：
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//第四个参数是可选的，允许你指定一个显示器来显示这个窗口。最后一个参数只与OpenGL有关。
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	}

	void initVulkan()
	{
		//创建实例
		createInstance();

		setupDebugMessenger();
	}

	void createInstance()
	{
		//需要用程序的一些信息去填充一个结构体。从技术上来说，这些信息可以不填，但是它们或许能够提供一些信息给驱动，以使驱动针对我们的特定程序进行优化
		VkApplicationInfo   appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;


		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		//指定窗口相接的扩展,GLFW已经集成了一个好用的内置函数，它返回GLFW需要的Vulkan扩展，我们可以直接把它传给Vulkan API：
		uint32_t  glfwExtensionCount = 0;
		const char** glfwExtensions = nullptr;
		//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		//createInfo.enabledExtensionCount = glfwExtensionCount;
		//createInfo.ppEnabledExtensionNames = glfwExtensions;
		//createInfo.enabledLayerCount = 0;

		//检查验证层
		if (enableValidationLayers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}
		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		//几乎每一个Vulkan函数的返回值都是 VkResult类型的，它要么是VK_SUCCESS，要么是一个错误代码
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("ckCreate instance failed!");
		}
	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	//用vkEnumerateInstanceLayerProperties函数列出所有可用的层
	bool checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;

	}

	std::vector<const char*> getRequiredExtensions()
	{
		//为了在创建实例之前得到所有受支持的扩展列表，可以用vkEnumerateInstanceExtensionProperties函数。它需要两个指针变量，
		//一个指向受支持的扩展数量，另一个指向一个VkExtensionProperties类型的、存储着扩展的细节的数组。它的第一个参数是可选的，允许我们使用一个特殊的验证层来选择扩展
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}


	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{

		if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			// Message is important enough to show
		}

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
		createInfo.pUserData = nullptr; // Optional
	}

	void setupDebugMessenger()
	{
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pCallback);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT callback, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, callback, pAllocator);
		}
	}

private:
	GLFWwindow* window;
	VkInstance  instance;
	VkDebugUtilsMessengerEXT debugMessenger; //Vulkan中的调试回调函数也由一个需要显式创建和销毁的句柄来管理,这种回调函数被称为“messenger”，
};

int main()
{
	HelloTriangleApplication app;

	try
	{
		app.run();
	}

	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
