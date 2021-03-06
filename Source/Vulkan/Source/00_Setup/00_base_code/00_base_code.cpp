#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
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
		1. 创建窗口
		2. 图形API操作
		2. 主循环
		3. 销毁窗口
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
		glfwDestroyWindow(window);

		glfwTerminate();
	}



private:
	GLFWwindow* window;

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
