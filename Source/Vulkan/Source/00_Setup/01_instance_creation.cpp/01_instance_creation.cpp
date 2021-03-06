#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

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
		3. 销毁资源（例如窗口，vkInstance）
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
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		//最后两个成员指定哪些全局验证层将会被启用
		createInfo.enabledLayerCount = 0;
		//几乎每一个Vulkan函数的返回值都是 VkResult类型的，它要么是VK_SUCCESS，要么是一个错误代码
		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("ckCreate instance failed!");
		}


		//lookupVkExtension();


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
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void lookupVkExtension()
	{
		//为了在创建实例之前得到所有受支持的扩展列表，可以用vkEnumerateInstanceExtensionProperties函数。它需要两个指针变量，
		//一个指向受支持的扩展数量，另一个指向一个VkExtensionProperties类型的、存储着扩展的细节的数组。它的第一个参数是可选的，允许我们使用一个特殊的验证层来选择扩展
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		for (const auto& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
		}
	}

private:
	GLFWwindow* window;
	VkInstance  instance;
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
