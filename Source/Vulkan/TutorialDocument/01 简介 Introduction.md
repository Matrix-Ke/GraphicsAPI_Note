# 简介

## 关于此教程

此教程将会教你使用基础的 [Vulkan](https://www.khronos.org/vulkan/) 图像及计算API。由 [Khronos group](https://www.khronos.org/) （因 OpenGL 而闻名）开发的Vulkan是一套全新的API，它提供了对现代显卡更好的抽象。这套新接口允许你更好地描述你的应用程序想做的事情，因此它拥有比现有的图形API，比如 [OpenGL](https://zh.wikipedia.org/wiki/OpenGL) 和 [Direct3D](https://zh.wikipedia.org/wiki/Direct3D) ，更好的性能以及更少的意外行为。Vulkan背后的思想比较接近 [Direct3D 12](https://zh.wikipedia.org/wiki/Direct3D#Direct3D_12) 和 [Metal](https://zh.wikipedia.org/wiki/Metal_(API)) ，但是 Vulkan 拥有完全跨平台的优点，也就是说，你可以同时为 Windows, Linux 和 Android 平台开发应用程序。

然而，为这些优点付出的代价是，你必须使用这些非常啰嗦的 API 。图形 API 中的每一个细节都需要你在使用前从零开始设置好，包括帧缓冲创建前的初始化，或者像是缓冲或者贴图这类对象的内存管理。显卡驱动程序少了很多初始化工作，这意味着你需要在你的应用程序中做更多工作来保证行为正确。

丑话先说在前头， Vulkan 并不适合每一个人。它针对的是那些钟情于高性能的计算机图形，并且愿意为其做贡献的程序员们。如果你对游戏开发更感兴趣而不是计算机图形学的话，你可能会愿意继续使用 OpenGL 或者 Direct3D 而不是抛弃它们换用 Vulkan 。另一个可供选择的选项是使用[虚幻4](https://zh.wikipedia.org/wiki/%E8%99%9A%E5%B9%BB%E5%BC%95%E6%93%8E#%E8%99%9A%E5%B9%BB%E5%BC%95%E6%93%8E4)或者[Unity](https://zh.wikipedia.org/wiki/Unity_(%E6%B8%B8%E6%88%8F%E5%BC%95%E6%93%8E))之类的游戏引擎，它们可以在使用 Vulkan 的同时暴露更高级的API给你。

说完了这些，来看看在接下来的教程中有什么事前准备需要做：
* 支持Vulkan的显卡及驱动([NVIDIA](https://developer.nvidia.com/vulkan-driver), [AMD](http://www.amd.com/en-us/innovations/software-technologies/technologies-gaming/vulkan), [Intel](https://software.intel.com/en-us/blogs/2016/03/14/new-intel-vulkan-beta-1540204404-graphics-driver-for-windows-78110-1540))
* C++使用经验（熟悉RAII和初始化列表）
* 支持C++11的编译器（Visual Studio 2017及以上版本，GCC 7及以上版本，或 Clang 5及以上版本）
* 对 3D 计算机图形学已有一些经验

此教程假设你不懂 OpenGL 或 Direct3D ，不过你的确需要知道一点 3D 计算机图形学的基本概念，比如此教程不会解释透视投影背后的数学原理。你可以读读[这本电子书](https://paroj.github.io/gltut/)，它很好地介绍了计算机图形学的概念。这是一些其它的关于计算机图形学的很棒的资源：
* [Ray tracing in one weekend](https://github.com/petershirley/raytracinginoneweekend)（《一个周末就能看懂的光线追踪》）
* [Physically Based Rendering book](http://www.pbr-book.org/)（《基于物理的渲染》）
* Vulkan在真正的游戏引擎中的实践，以开源版本的[Quake](https://github.com/Novum/vkQuake)和[DOOM 3](https://github.com/DustinHLand/vkDOOM3)为例

如果你想的话，你可以用 C 代替 C++ 。不过你得用一个其它的线性代数库，并且代码结构也得用你自己的。我们将会使用一些C++的特性，比如类和RAII，来组织代码逻辑并管理资源的生命周期。本教程还有针对 Rust 开发者的[另外一个版本](https://github.com/bwasty/vulkan-tutorial-rs)。

为了让那些使用其他语言的开发者们能够跟上我们的教程，也为了能够积攒一点使用基础API的经验，我们将会使用Vulkan的原生 C API 。不过如果你在使用C++，或许你会更喜欢这个比较新的 [Vulkan-Hpp](https://github.com/KhronosGroup/Vulkan-Hpp) 绑定，它对一些脏活累活做了抽象，并且有助于防止某些错误的发生。

## 电子书
如果你更喜欢以电子书的形式阅读此教程，你可以在此下载EPUB或者PDF格式的版本（英文原文）：
* [EPUB](https://raw.githubusercontent.com/Overv/VulkanTutorial/master/ebook/Vulkan%20Tutorial.epub)
* [PDF](https://raw.githubusercontent.com/Overv/VulkanTutorial/master/ebook/Vulkan%20Tutorial.pdf)

## 此教程的结构

我们首先概述 Vulkan 的工作原理，然后介绍在屏幕上画出第一个三角形需要做的所有工作。其目的在于，当你理解了每一个小步骤在整个流程中的基本作用之后，你会更好地理解它们。下一步，我们会用 [Vulkan SDK](https://lunarg.com/vulkan-sdk/) 、进行线性代数运算的 [GLM](http://glm.g-truc.net/) 库和创建窗口的 [GLFW](http://www.glfw.org/) 库建立开发环境。此教程将会展示如何在 Windows & Visual Studio 和 Ubuntu Linux & GCC 上设置这些库。

然后，我们将会实现所有必要的 Vulkan 程序组件，用来渲染你的第一个三角形。每一章都会大致遵循以下结构：
* 介绍一个新概念以及它的作用
* 调用所有相关的 API 来把它整合进你的程序里
* 将它的一部分抽象为辅助函数

虽然此教程的每一章都与上一章相连，但是你也可以把这些章节作为独立的文章来阅读，每篇文章讲解某个 Vulkan 的特性。这意味着，这个网站也可以作为参考资料使用。所有 Vulkan 函数和类型都被链接到了它们的规范上，你可以点击它们来获取更多内容。 Vulkan 是一套非常新的API，所以规范本身也有可能出现某些不足，我们鼓励你向[这个 Khronos 仓库](https://github.com/KhronosGroup/Vulkan-Docs)提交反馈。

正如之前所说过的， Vulkan API 是一个有许多参数的、相当啰嗦的 API ，旨在能让你最大限度地控制显卡。这就导致了哪怕是基础操作，例如创建一个纹理，都要经过很多步骤，并且每次都要重复这些步骤。因此我们会在教程的每一步创建我们自己的辅助函数合集。

每一章的最后都会有一个链接，指向到目前为止的完整代码。如果你对代码的结构有疑问，或者你在 debug 时需要一个参考对象，你都可以来参考这个源代码。所有代码都经过了多家厂商的不同显卡来验证其正确性。在每一章的底部都有一个评论区，你可以在这问关于本章的具体问题。提问时请说明你的平台、驱动版本、源代码、预期行为和实际行为来让我们帮助你。

这个教程旨在结成一个社区。Vulkan现在仍然是一套非常新的API，它的最佳实践现在还没有形成。如果你对此教程或此网站有任何反馈，不要迟疑，提交一个 issue 或者一个 pull request 到[这个 GitHub 仓库](https://github.com/Overv/VulkanTutorial)。你可以watch这个仓库以获取此教程的更新提示。

当你在屏幕上渲染出来了第一个三角形之后，我们会扩展这个程序，将线性变换、纹理和 3D 模型加入进去。

如果你在这之前玩过图形 API ，你就会知道在屏幕上渲染出几何图形之前可能有许多要做的步骤。这种初始化步骤在 Vulkan 中有很多，不过你会发现每个具体步骤都很容易理解，并且也不会感到多余。还有一件很重要的事情，那就是要记住，当你渲染出了那个看起来很无聊的三角形之后，绘制有纹理的 3D 模型并不需要做太多的额外工作，并且向前迈进的每一步都会有许多收获。

如果你在跟着教程操作的时候遇到了问题，首先请查看“常见问题”部分是否已经列出并解决了你的问题。如果没有，请放轻松并且在最相关的那一章的评论区提问。

准备好深入了解高性能图形API的未来了吗？让我们开始吧！



步骤有很多，但是在接下来的章节中，每一步的目标都会变得非常简单而清晰。如果对某一步在整个程序中的作用有疑惑，应该回来参考本章。

## API概念

本章将简要概述Vulkan API在更低的级别上的结构。


### 代码约定

Vulkan中所有的函数、枚举类型和结构体都定义在了`vulkan.h`头文件中，这个文件在 LunarG 开发的 [Vulkan SDK](https://lunarg.com/vulkan-sdk/) 里。下一章将会介绍如何安装这个SDK。

函数以小写的`vk`开头，枚举类型和结构体以`Vk`开头，枚举值则以`VK_`开头。这套API非常依赖结构体作为函数参数。举个例子，对象通常以这种形式创建：

```c++
VkXXXCreateInfo createInfo = {};
createInfo.sType = VK_STRUCTURE_TYPE_XXX_CREATE_INFO;
createInfo.pNext = nullptr;
createInfo.foo = ...;
createInfo.bar = ...;

VkXXX object;
if (vkCreateXXX(&createInfo, nullptr, &object) != VK_SUCCESS) {
    std::cerr << "failed to create object" << std::endl;
    return false;
}
```

Vulkan中的许多结构体要求在`sType`成员中明确指定结构体类型。`pNext`成员可以是一个指向扩展结构的指针，在此教程中它将被永远置为`nullptr`。创建或销毁一个对象的函数会有一个[`VkAllocationCallbacks`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkAllocationCallbacks.html)参数，允许为启动内存使用一个自定义的分配器，它在此教程中也将永远被置为`nullptr`。

几乎所有函数的返回值都是一个[`VkResult`](https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkResult.html)的枚举类型，它要么是`VK_SUCCESS`（成功），要么是一个错误代码。 Vulkan 规范说明了每个函数会返回什么错误代码以及它们的含义。

### 验证层

就像之前说过的， Vulkan 被设计为一个高性能低负载的 API 。因此它默认的错误检查和调试能力非常有限。当做错了什么的时候，驱动程序常常是直接崩溃而不是返回一个错误代码——或者更糟糕的是，在的显卡上跑得起来，在别的显卡上就完全不行了。

可以通过“验证层”（validation layers）来给 Vulkan 启用一个扩展的错误检查功能。验证层是一些可以被插入到 API 与显卡驱动之间的代码片段，可以用来运行额外的函数参数检查或者追踪内存管理问题。它的优点是可以在开发的时候启用验证层，然后在发行版本中完全禁用它以避免性能开销。每个人都可以编写自己的验证层，不过 LunarG 开发的 Vulkan SDK 提供了一些标准的验证层，在教程中用到的就是它们。为了从验证层接收调试信息，需要注册一个回调函数。

Vulkan中每个操作都非常明确，并且还有如此灵活的验证层可用，因此事实上 Vulkan 比 OpenGL 和 Direct3D 更容易找到错误原因。

在开始写代码之前只有一步要做了，那就是配置开发环境。