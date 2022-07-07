# 开发环境

这一章我们会配置一个 Vulkan 应用程序并且安装一些实用的库。我们用到的所有工具，除了编译器之外都是同时兼容 Windows, Linux 和 MacOS 的，不过安装步骤会有一些不同，也正是因此在这一章里我们详细地讲解这些工具。

## Windows

如果你在 Windows 下开发，那么我假设你用的是 Visual Studio 来编译代码。如果要获得完整的 C++17 支持，你需要使用 Visual Studio 2017 或 2019. 下面的步骤是为 VS 2017 编写的。

### Vulkan SDK

开发 Vulkan 应用程序时最重要的组件就是 Vulkan SDK. 它包括了头文件、标准验证层、调试工具以及加载 Vulkan 函数用的一个加载器。这个加载器在运行时从显卡驱动中加载一个函数，就像是 GLEW 之于 OpenGL 那样——如果你熟悉它们的话。

你可以在 [LunarG 网站](https://vulkan.lunarg.com/)底部的按钮处下载这个SDK。不需要你创建账号，但是有一个账号可以让访问一些对你来说可能会有用的文档。

![](https://vulkan-tutorial.com/images/vulkan_sdk_download_buttons.png)

安装时记下SDK的安装位置。首先我们要确认一下你的显卡和驱动是否正确地支持Vulkan。进入SDK安装目录，打开 `Bin` 文件夹，运行 `cude.exe` ，你看到的应该是如下画面：

![](https://vulkan-tutorial.com/images/cube_demo.png)

如果你收到了一个错误提示，确保你的驱动是最新的，以及你的显卡的确支持 Vulkan 运行时。参阅简介那一章来获取主要显卡供应商的驱动下载地址。

这个文件夹里还有一个在开发时会很有用的程序。`glslangValidator.exe` 程序可以把人类可读的 [GLSL](https://zh.wikipedia.org/wiki/GLSL) 编译成字节码。我们会在着色器模块那一章详细讲解这个程序。 `Bin `文件夹中也有关于 Vulkan 加载器和验证层的二进制程序，然后`Lib`文件夹中是库。

`Doc `文件夹里面有很多关于 Vulkan SDK 的实用信息，以及一个 Vulkan 规范的离线版本。最后， `Include` 文件夹中包含了 Vulkan 头文件。你可以自由探索其它的文件夹里都有什么，不过我们在此教程中用不到它们。

### GLFW

就像之前提过的， Vulkan 本身是一套平台无关的 API ，并且没有任何关于创建窗口并在上面显示渲染结果这类的工具。为了能体现出 Vulkan 跨平台的优点，并且绕开恐怖的 Win32, 我们会用兼容 Windows, Linux 和 MacOS 的 [GLFW 库](http://www.glfw.org/)来创建窗口。虽然也有别的库可以用，比如 [SDL](https://www.libsdl.org/) ，不过GLFW的优点在于，除了单纯的创建窗口以外，它把另外一些 Vulkan 中与具体平台相关的东西也抽象掉了。

你可以在 GLFW 的[官网](http://www.glfw.org/download.html)找到 GLFW 的最新发行版。在此教程中我们使用 64 位版本，不过你也可以用 32 位版本。如果你要用 32 位版本，那么在链接 Vulkan SDK 的时候就要选择 `Lib32` 文件夹里的库而不是 `Lib` 文件夹里的。下载好之后，把它解压到一个方便的位置。我选择在“文档”下的 Visual Studio 文件夹里创建一个 `Libraries` 文件夹。

![](https://vulkan-tutorial.com/images/glfw_directory.png)

### GLM

与 DirectX 12 不同， Vulkan 没有自带的线性代数库，所以我们需要自己下载一个。 [GLM](http://glm.g-truc.net/) 是一个不错的库，对图形 API 友好，并且已经在 OpenGL 开发中广泛应用。

GLM 是一个只有头文件的库，所以只需要下载[最新版本](https://github.com/g-truc/glm/releases)然后把它放在一个方便的位置。你的目录结构现在应该类似下图：

![](https://vulkan-tutorial.com/images/library_directory.png)

### 配置 Visual Studio

现在你已经安装好了所有的依赖，我们可以为 Vulkan 配置一个基础的 Visual Studio 项目然后写一点代码来确定每个部分是否都在正常工作。

启动 Visual Studio ，新建一个 `Windows 桌面向导`项目，起一个项目名字，然后点击“确定”。

![](https://vulkan-tutorial.com/images/vs_new_cpp_project.png)

确保你的应用类型是`控制台应用(.exe)`，这样我们就有了一个地方来输出调试信息，然后选中`空项目`。这样可以防止 Visual Studio 添加样板代码。

![](https://vulkan-tutorial.com/images/vs_application_settings.png)

点击“确定”按钮来创建项目，然后添加一个C++源文件。你应该已经知道应该怎么做了，不过为了完整，此处列出相应步骤。

![](https://vulkan-tutorial.com/images/vs_new_item.png)

![](https://vulkan-tutorial.com/images/vs_new_source_file.png)

现在我们在文件中写入下列代码。现在看不懂这些代码也没关系。我们只是确定一下你能否编译并且运行 Vulkan 应用程序。在下一章我们会从头开始的。

```cpp
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported" << std::endl;
    
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    
    return  0;
}
```

现在配置一下项目来避免出错。打开项目属性对话框并且确保选择的是`所有配置`，因为很多设置都需要同时在 `Debug` 和 `Release` 模式下生效。

![](https://vulkan-tutorial.com/images/vs_open_project_properties.png)

![](https://vulkan-tutorial.com/images/vs_all_configs.png)

进入到`C/C++ → 常规 → 附加包含目录`然后点击下拉列表里的`<编辑...>`。

![](https://vulkan-tutorial.com/images/vs_cpp_general.png)

然后添加 Vulkan, GLFW 和 GLM 的头文件路径：

![](https://vulkan-tutorial.com/images/vs_include_dirs.png)

接下来，在`链接器 → 常规 → 附加库目录`里面设置库目录。

![](https://vulkan-tutorial.com/images/vs_link_settings.png)

然后把 Vulkan 和 GLFW 的库路径添加进去：

![](https://vulkan-tutorial.com/images/vs_link_dirs.png)

进入到`链接器 → 输入 → 附加依赖项`然后点击下拉列表里的`<编辑...>`。

![](https://vulkan-tutorial.com/images/vs_link_input.png)

输入 Vulkan 和 GLFW 的对象文件：

![](https://vulkan-tutorial.com/images/vs_dependencies.png)

最后启用 C++17 支持：（`C/C++ → 语言 → C++语言标准`）

![](https://vulkan-tutorial.com/images/vs_cpp17.png)

现在你可以关掉项目属性对话框了。如果你的每一步都做对了，代码中所有的高亮错误提示都应该消失了。

最后，确保你在使用 64 位编译器：

![](https://vulkan-tutorial.com/images/vs_build_mode.png)

按下 `F5` 来编译并运行程序，你应该看到一个命令提示符窗口，和一个在它上方的窗口，就像这样：

![](https://vulkan-tutorial.com/images/vs_test_window.png)

插件（extension）的数量应用是一个非0值。恭喜你，你已经准备好去探索Vulkan了！

## Linux

这个部分是针对 Ubuntu 用户的，不过你也可以跟着这个部分编译 LunarG SDK ，然后把那些`apt`命令换成你自己的包管理器的。你应该已经安装好了支持 C++17 的编译器（GCC 7+ 或 Clang5+）。你还需要make。

### Vulkan 软件包

在 Linux 上开发 Vulkan 应用程序所需要的最重要的组件是 Vulkan 加载器、验证层和几个测试你的机器是否支持 Vulkan 的命令行实用程序。

* `sudo apt install vulkan-tools`: 命令行实用程序，最重要的是 `vulkaninfo` 和 `vkcube`. 运行这些程序开确认你的机器是否支持 Vulkan.

* `sudo apt install libvulkan-dev`: 安装 Vulkan 加载器。这个加载器在运行时从显卡驱动中加载一个函数，就像是 GLEW 之于 OpenGL 那样——如果你熟悉它们的话。

* `sudo apt install vulkan-validationlayers-dev spirv-tools`: 安装标准验证层和所需的 SPIR-V 工具。这些东西在调试 Vulkan 应用程序时至关重要，我们会在下一章介绍它们。

如果安装成功，你应该已经完成了 Vulkan 部分的安装。记得运行一下 `vkcube` , 并确保你在窗口中看到以下弹出的内容：

![](https://vulkan-tutorial.com/images/cube_demo_nowindow.png)

如果你收到了一个错误提示，确保你的驱动是最新的，以及你的显卡的确支持Vulkan运行时。阅读简介那一章来获取主要显卡供应商的驱动下载地址。

### GLFW

就像之前提过的， Vulkan 本身是一套平台无关的 API ，并且没有任何关于创建窗口并在上面显示渲染结果这类的工具。为了能体现出 Vulkan 跨平台的优点，并且绕开恐怖的 X11, 我们会用兼容 Windows, Linux 和 MacOS 的 [GLFW 库](http://www.glfw.org/)来创建窗口。虽然也有别的库可以用，比如 [SDL](https://www.libsdl.org/) ，不过GLFW的优点在于，除了单纯的创建窗口以外，它把另外一些 Vulkan 中与具体平台相关的东西也抽象掉了。

我们会用以下命令来安装 GLFW.

```shell
sudo apt install libglfw3-dev
```

### GLM

与 DirectX 12 不同， Vulkan 没有自带的线性代数库，所以我们需要自己下载一个。 [GLM](http://glm.g-truc.net/) 是一个不错的库，对图形 API 友好，并且已经在 OpenGL 开发中广泛应用。

这是一个只有头文件的库，可以通过安装 `libglm-dev` 包来安装。

```bash
sudo apt install libglm-dev
```

### 着色器编译器

我们基本上已经安装完了我们需要的所有内容，除了一个可以把人类可读的 [GLSL](https://zh.wikipedia.org/wiki/GLSL) 编译成字节码的编译器。

有两个很流行的编译器，一个是 Khronos Group 开发的 `glslangValidator`, 另一个是谷歌开发的`glslc`. 后者的用法比较接近 GCC 和 Clang, 所以我们决定用这个：下载谷歌的[非官方二进制文件](https://github.com/google/shaderc/blob/main/downloads.md)并且复制 `glslc` 文件到你的 `/usr/local/bin` 目录。注意，取决于你的用户权限，你可能需要用 `sudo`. 要测试的话，直接运行 `glslc` 然后它应该理所当然地抱怨没有传入任何着色器以供编译：

`glslc: error: no input files`

我们会在着色器模块一章中深入介绍 `glslc`.

### 配置 makefile 项目

现在你已经安装好了所有依赖，我们可以为 Vulkan 配置一个基础的 makefile 项目然后写一点代码来确定每个部分都在正常工作。

在方便的地方创建一个新文件夹，起个名字，比如 `VulkanTest`. 创建一个名为 `main.cpp` 的 C++ 源文件并且写入如下代码。现在看不懂这些代码也没关系。我们只是确定一下你能否编译并且运行 Vulkan 应用程序。在下一章我们会从头开始的。

```cpp
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
int main() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);
    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << extensionCount << " extensions supported" << std::endl;
    
    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;
    
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    
    return  0;
}
```

接下来，我们会编写一个用来编译和运行这些代码的 makefile. 创建一个新的空文件并命名为 `Makefile`. 我默认你已经有了一些编写 makefile 的基本经验，比如变量和规则是怎么工作的。如果你不懂 makefile, 你可以看看[这份教程](http://mrbook.org/blog/tutorials/make/)来快速入门。

我们首先来定义几个变量来简化一下这个文件。定义一个 `CFLAGS` 变量来指定编译器的基本标记：

```makefile
CFLAGS = -std=c++17 -O2
```

我们将使用现代 C++ (-std=c++17), 并把优化级别设置为O2. 可以去掉 -O2 以加快编译速度，但是在发行版中要记得加回去。

同样地，在 `LDFLAGS` 变量中定义链接器标记：

```makefile
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
```

`-lglfw` 标记代表 GLFW, `-lvulkan` 标记代表与 Vulkan 函数加载器链接，其余标记是一些 GLFW 需要的低级系统库。其余标记是 GLFW 自身的依赖库：线程库和窗口管理器。

现在来指定编译 `VulkanTest` 的规则，这个规则就非常一目了然了。确保使用制表符进行缩进而不是空格：

```make
VulkanTest: main.cpp
    g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)
```

保存makefile并在`main.cpp`和`Makefile`的目录下运行`make`命令来验证一下这个规则是否正常工作。它应该最终生成一个`VulkanTest`可执行文件。

现在我们再来添加两条规则：`test`和`clean`，前者运行编译出的可执行文件，后者则删除这个可执行文件。

```make
.PHONY: test clean

test: VulkanTest
    ./VulkanTest

clean:
    rm -f VulkanTest
```

运行 `make test` 命令应该显示程序运行成功，并且显示出Vulkan插件（extension）的数量。这个程序在你关闭空窗口后应该退出并返回成功的返回代码（`0`）。完整的 makefile 应该类似于这样：

```make
CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

VulkanTest: main.cpp
    g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
    rm -f VulkanTest
```

你可以把这个文件夹当成一个 Vulkan 项目的模板。复制或重命名成 `HelloTriangle` 或者别的什么名字，然后删掉`main.cpp`里的所有代码。

现在，你已经完全准备好进行真正的探险了。

## MacOS

*译者：我没用过MacOS，万一翻译出现了偏差我是要负责的，大体上和上面两个差不多，反正MacOS也是类UNIX系统，我寻思跟Linux应该差不多（逃）*