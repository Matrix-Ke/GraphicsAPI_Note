# 描述符布局和缓冲
## 介绍
现在，我们已经可以传递顶点属性给顶点着色器，但对于一些所有顶点都共享的属性，比如顶点的变换矩阵，将它们作为顶点属性为每个顶点都传递一份显然是非常浪费的。
Vulkan提供了资源描述符来解决这一问题。描述符是用来在着色器中访问缓冲和图像数据的一种方式。我们可以将变换矩阵存储在一个缓冲中，然后通过描述符在着色器中访问它。使用描述符需要进行下面三部分的设置：
1. 在管线创建时指定描述符布局
2. 从描述符池分配描述符集
3. 在渲染时绑定描述符集
描述符布局用于指定可以被管线访问的资源类型，类似渲染流程指定可以被访问的附着类型。描述符集指定了要绑定到描述符上的缓冲和图像资源，类似帧缓冲指定绑定到渲染流程附着上的图像视图。最后，将描述符集绑定到绘制指令上，类似绑定顶点缓冲和帧缓冲到绘制指令上。

有多种类型的描述符，但在本章节，我们只使用uniform缓冲对象(UBO)。在之后的章节，我们会看到其它类型的描述符，它们的使用方式和uniform缓冲对象类似。现在，让我们先用结构体定义我们要在着色器中使用的uniform数据：
```c++
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
```
我们将要使用的uniform数据复制到一个VkBuffer中，然后通过一个uniform缓冲对象描述符在顶点着色器中访问它：
```c++
layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model *
	vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
```
接下来，我们在每一帧更新模型，视图，投影矩阵，来让上一章节渲染的矩形在三维空间中旋转。

## 顶点着色器
修改顶点着色器包含上面的uniform缓冲对象。这里，我们假定读者对MVP变换矩阵有一定了解。如果没有，读者可以查找资源学习一下。
```c++
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
```
uniform，in和out定义在着色器中出现的顺序是可以任意的。上面代码中的binding修饰符类似于我们对顶点属性使用的location修饰符。我们会在描述符布局引用这个binding值。包含gl_Position的那行现在使用变换矩阵计算顶点最终的裁剪坐标。

描述符集布局
下一步，我们在应用程序的代码中定义UBO：
```c++
struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};
```

通过GLM库我们可以准确地匹配我们在着色器中使用变量类型，可以放心地直接使用memcpy函数复制UniformBufferObject结构体的数据VkBuffer中。
我们需要在管线创建时提供着色器使用的每一个描述符绑定信息。我们添加了一个叫做createDescriptorSetLayout的函数，来完成这项工作。并在管线创建前调用它：
```C++
void initVulkan() {
		...
	createDescriptorSetLayout();
	createGraphicsPipeline();
		...
}

		...

void createDescriptorSetLayout() {

}
我们需要使用VkDescriptorSetLayoutBinding结构体来描述每一个绑定：

void createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
}
```
The first two fields specify the `binding` used in the shader and the type of descriptor, which is a uniform buffer object. It is possible for the shader variable to represent an array of uniform buffer objects, and `descriptorCount` specifies the number of values in the array. This could be used to specify a transformation for each of the bones in a skeleton for skeletal animation, for example. Our MVP transformation is in a single uniform buffer object, so we're using a `descriptorCount` of `1`.
binding和descriptorType成员变量用于指定着色器使用的描述符绑定和描述符类型。这里我们指定的是一个uniform缓冲对象。着色器变量可以用来表示uniform缓冲对象数组，descriptorCount成员变量用来指定数组中元素的个数。我们可以使用数组来指定骨骼动画使用的所有变换矩阵。在这里，我们的MVP矩阵只需要一个uniform缓冲对象，所以我们将descriptorCount的值设置为1。
```c++
uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
```
我们还需要指定描述符在哪一个着色器阶段被使用。stageFlags成员变量可以指定通过VkShaderStageFlagBits组合或VK_SHADER_STAGE_ALL_GRAPHICS指定描述符被使用的着色器阶段。在这里，我们只在顶点着色器使用描述符。
```c++
 uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
```
pImmutableSamplers成员变量仅用于和图像采样相关的描述符。这里我们先将其设置为默认值，之后的章节会对它进行介绍。

所有的描述符绑定被组合进一个VkDescriptorSetLayout对象。我们在pipelineLayout成员变量的定义上面定义descriptorSetLayout成员变量：
```c++
VkDescriptorSetLayout descriptorSetLayout;
VkPipelineLayout pipelineLayout
//调用vkCreateDescriptorSetLayout函数创建VkDescriptorSetLayout对象。vkCreateDescriptorSetLayout函数以VkDescriptorSetLayoutCreateInfo结构体作为参数：
VkDescriptorSetLayoutCreateInfo layoutInfo = {};
layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
layoutInfo.bindingCount = 1;
layoutInfo.pBindings = &uboLayoutBinding;

if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr,	
				&descriptorSetLayout) != VK_SUCCESS) {
	throw std::runtime_error("failed to create descriptor set layout!");
}
```
我们需要在管线创建时指定着色器需要使用的描述符集布局。管线布局对象指定了管线使用的描述符集布局。修改VkPipelineLayoutCreateInfo结构体信息引用布局对象：
```c++
VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
pipelineLayoutInfo.setLayoutCount = 1;
pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
```
读者可能会有疑问，明明一个VkDescriptorSetLayout对象就包含了所有要使用的描述符绑定，为什么这里还可以指定多个VkDescriptorSetLayout对象，我们会在下一章节作出解释。
描述符布局对象可以在应用程序的整个生命周期使用，即使使用了新的管线对象。通常我们在应用程序退出前才清除它：
```c++
void cleanup() {
	cleanupSwapChain();

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

		...
}
```

## uniform缓冲
在下一章节，我们将会为着色器指定包含UBO数据的缓冲对象。我们首先需要创建用于包含数据的缓冲对象，然后在每一帧将新的UBO数据复制到uniform缓冲。由于需要频繁的数据更新，在这里使用暂存缓冲并不会带来性能提升。

由于我们同时并行渲染多帧的缘故，我们需要多个uniform缓冲，来满足多帧并行渲染的需要。我们可以对并行渲染的每一帧或每一个交换链图像使用独立的uniform缓冲对象。由于我需要在指令缓冲中引用uniform缓冲，对于每个交换链图像使用独立的uniform缓冲对象相对来说更加方便。

添加两个新的类成员变量uniformBuffers和uniformBuffersMemory：
```c++
VkBuffer indexBuffer;
VkDeviceMemory indexBufferMemory;

std::vector<VkBuffer> uniformBuffers;
std::vector<VkDeviceMemory> uniformBuffersMemory;
```
添加一个叫做createUniformBuffers的函数，在createIndexBuffer函数调用后调用它分配uniform缓冲对象：
```c++
void initVulkan() {
		...
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
		...
}

		...

void createUniformBuffer() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					uniformBuffers[i], uniformBuffersMemory[i]);
	}
}
//我们会在另外的函数中使用新的变换矩阵更新uniform缓冲，所以在这里没有出现vkMapMemory函数调用。应用程序退出前不要忘记清除申请的uniform缓冲对象：
void cleanup() {
	cleanupSwapChain();

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

		...
}
```
## 更新uniform数据
添加一个叫做updateUniformBuffer的函数，然后在drawFrame函数中我们已经可以确定获取交换链图像是哪一个后调用它：
```c++
void drawFrame() {
		...

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(),
						imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
						&imageIndex);

		...

	updateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		...
}

		...

void updateUniformBuffer(uint32_t currentImage) {

}
``` 
调用updateUniformBuffer函数可以在每一帧产生一个新的变换矩阵。updateUniformBuffer函数的实现需要使用了下面这些头文件：
```c++
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
```
包含glm/gtc/matrix_transform.hpp头文件是为了使用glm::rotate之类的矩阵变换函数。GLM_FORCE_RADIANS宏定义用来使glm::rotate这些函数使用弧度作为参数的单位。

包含chrono头文件是为了使用计时函数。我们将通过计时函数实现每秒旋转90度的效果。
```c++
void updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float,
			std::chrono::seconds::period>(currentTime - startTime).count();
}
```
我们在uniform缓冲对象中定义我们的MVP变换矩阵。模型的渲染被我们设计成绕Z轴渲染time弧度。
```c++
UniformBufferObject ubo = {};
ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), 
				glm::vec3(0.0f, 0.0f, 1.0f));
// glm::rotate函数以矩阵，旋转角度和旋转轴作为参数。glm::mat4(1.0f)用于构造单位矩阵。这里，我们通过time * glm::radians(90.0f)完成每秒旋转90度的操作。
ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
			glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
// 对于视图变换矩阵，我们使用上面代码中的定义。glm::lookAt函数以观察者位置，视点坐标和向上向量为参数生成视图变换矩阵。
ubo.proj = glm::perspective(glm::radians(45.0f),
			swapChainExtent.width / (float) swapChainExtent.height,
			0.1f, 10.0f);
ubo.proj[1][1] *= -1;
```
对于透视变换矩阵，我们使用上面代码中的定义。glm::perspective函数以视域的垂直角度，视域的宽高比以及近平面和远平面距离为参数生成透视变换矩阵。特别需要注意在窗口大小改变后应该使用当前交换链范围来重新计算宽高比。

GLM库最初是为OpenGL设计的，它的裁剪坐标的Y轴和Vulkan是相反的。我们可以通过将投影矩阵的Y轴缩放系数符号取反来使投影矩阵和Vulkan的要求一致。如果不这样做，渲染出来的图像会被倒置。

定义完所有的变换矩阵，我们就可以将最后的变换矩阵数据复制到当前帧对应的uniform缓冲中。复制数据的方法和复制顶点数据到顶点缓冲一样，除了没有使用暂存缓冲：
```c++
void* data;
vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
memcpy(data, &ubo, sizeof(ubo));
vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
```
对于在着色器中使用的需要频繁修改的数据，这样使用UBO并非最佳方式。还有一种更加高效的传递少量数据到着色器的方法，我们会在之后的章节介绍它。

在下一章节，我们将会对绑定VkBuffer对象到uniform缓冲描述符的描述符集进行介绍。