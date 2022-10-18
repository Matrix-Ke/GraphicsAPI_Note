//应用程序框架
#pragma once
//此处显示指出使用vulkan
#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h> 

#include <array>
#include <functional>
#include <vector>

namespace Matrix
{
	struct WindowCreateInfo
	{
		int         width{ 1280 };
		int         height{ 720 };
		const char* title{ "Matrix" };
		bool        is_fullscreen{ false };
	};


	class WindowSystem
	{
	public:
		WindowSystem();
		~WindowSystem();


		//base function
		void               initialize(WindowCreateInfo create_info);
		void               pollEvents() const;
		bool               shouldClose() const;
		void               setTitle(const char* title);
		GLFWwindow* getWindow() const;
		std::array<int, 2> getWindowSize() const;

		//键盘鼠标事件



	private:

	};
}