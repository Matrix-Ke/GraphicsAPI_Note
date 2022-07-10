# 介绍
在接下来的章节，我们会将之前在顶点着色器中直接硬编码的顶点数据替换为顶点缓冲来定义。我们首先创建一个CPU的缓冲，然后将我们要使用的顶点数据先复制到这个CPU缓冲中，最后，我们复制CPU缓冲中的数据到阶段缓冲。

## 顶点着色器
修改顶点着色器，去掉代码中包含的硬编码顶点数据。使用in关键字使用顶点缓冲中的顶点数据：
```c++
#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

out gl_PerVertex {
	vec4 gl_Position;
};

void main() {
	gl_Position = vec4(inPosition, 0.0, 1.0);
	fragColor = inColor;
}
```
上面代码中的inPosition和inColor变量是顶点属性。它们代表了顶点缓冲中的每个顶点数据，就跟我们使用数组定义的顶点数据是一样的。重新编译顶点着色器，保证没有出现问题。

layout(location = x)用于指定变量在顶点数据中的索引。特别需要注意，对于64位变量，比如dvec3类型的变量，它占用了不止一个索引位置，我们在定义这种类型的顶点属性变量之后的顶点变量，要注意索引号的增加并不是1：
```c++
layout(location = 0) in dvec3 inPosition;
layout(location = 2) in vec3 inColor;
```
读者可以查阅OpenGL的官方文档获得关于layout修饰符的更多信息。

## 顶点数据
我们将顶点数据从顶点着色器代码移动到我们的应用程序的代码中。我们首先包含GLM库的头文件，它提供了我们需要使用的线性代数库：
```c++
#include <glm/glm.hpp>
// 创建一个叫做Vertex的新的结构体类型，我们使用它来定义单个顶点的数据：

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;
};
```
GLM库提供了能够完全兼容GLSL的C++向量类型：
```c++
const std::vector<Vertex> vertices = {
				{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
				{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};
```
现在，我们可以使用Vertex结构体数组来定义我们的顶点数据。这次定义和之前在顶点着色器中进行的略有不同，之前在顶点着色器中，我们将顶点位置和顶点颜色数据定义在了不同的数组中，这次，我们将它们定义在了同一个结构体数组中。这种顶点属性定义方式定义的顶点数据也被叫做交叉顶点属性(interleaving vertex attributes)。

## 绑定描述
定义好顶点数据，我们需要将CPU缓冲(也就是我们定义的Vertex结构体数组)中顶点数据的存放方式传递给GPU，以便GPU可以正确地加载这些数据到显存中。

我们给Vertex结构体添加一个叫做getBindingDescription的静态成员函数，在里面编写代码返回Vertex结构体的顶点数据存放方式：
```c++
struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription
	getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};

		return bindingDescription;
	}
};
```
我们使用VkVertexInputBindingDescription结构体来描述CPU缓冲中的顶点数据存放方式。描述的数据包括：每个条目的数据所占的字节数，连续两个条目的数据之前的字节跨距，以及数据的粒度。
```c++
VkVertexInputBindingDescription bindingDescription = {};
bindingDescription.binding = 0;
bindingDescription.stride = sizeof(Vertex);
bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
```
每个顶点的所有数据被包装到同一个结构体数组，所以我们只需要填写一个绑定描述信息。binding成员变量用于指定绑定描述信息在绑定描述信息数组中的索引。stride成员变量用于指定条目之间以字节为大小的跨距。inputRate成员变量用于指定条目的粒度，它可以是下面这些值：

VK_VERTEX_INPUT_RATE_VERTEX：以一个顶点数据作为条目的粒度
VK_VERTEX_INPUT_RATE_INSTANCE：以一个实例数据作为条目的粒度
在这里，我们没有使用实例渲染，所以选择使用VK_VERTEX_INPUT_RATE_VERTEX作为数据的条目粒度。

## 属性描述
我们使用VkVertexInputAttributeDescription结构体来描述顶点属性，我们给Vertex结构体类型添加另一个静态成员函数来返回顶点属性描述信息：
```c++
#include <array>

		...

static std::array<VkVertexInputAttributeDescription, 2>
getAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2>
	attributeDescriptions = {};

	return attributeDescriptions;
}
```
如上面代码所示，我们使用了两个VkVertexInputAttributeDescription结构体来分别描述我们的顶点位置和顶点颜色属性信息。
```c++
attributeDescriptions[0].binding = 0;
attributeDescriptions[0].location = 0;
attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
attributeDescriptions[0].offset = offsetof(Vertex, pos);
```
binding成员变量用于指定顶点数据来源。location成员变量用于指定属性在顶点着色器中的存放位置。这里我们将顶点位置属性的location设置为0。

format成员变量用于指定顶点属性的数据类型。它可以是下面这些值：
float：VK_FORMAT_R32_SFLOAT
vec2：VK_FORMAT_R32G32_SFLOAT
vec3：VK_FORMAT_R32G32B32_SFLOAT
vec4：VK_FORMAT_R32G32B32A32_SFLOAT
我们应该使用与我们在顶点着色器中定义的顶点属性变量类型的颜色通道数量相兼容的格式。当然，指定使用比在着色器中定义的类型更多通道数量的格式也是可以的，但多出来的通道会被丢弃掉。如果指定格式的通道数小于着色器中的定义，BGA通道会使用(0，0，1)作为多出来的通道的值。颜色数据类型(SFLOAT，UINT，SINT)和位宽也应该使用和着色器定义相兼容的。下面是一些使用举例：

ivec2：VK_FORMAT_R32G32_SINT，具有两个分量的，每个分量类型是32位符号整型的向量
uvec4：VK_FORMAT_R32G32B32A32_UINT，具有四个分量的，每个分量类型是32位无符号整型的向量
double：VK_FORMAT_R64_SFLOAT，一个双精度(64位)浮点数
format成员变量隐含指定了每个顶点属性数据所占字节大小。offset成员变量指定了开始读取顶点属性数据的位置。使用它是因为，我们的顶点数据包含了多个顶点属性数据，它们并不都是存储在整个顶点数据的开始位置。我们这里使用offsetof宏计算颜色数据在整个顶点数据中的开始位置：
```c++
attributeDescriptions[1].binding = 0;
attributeDescriptions[1].location = 1;
attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
attributeDescriptions[1].offset = offsetof(Vertex, color);
```
颜色数据的描述与之类似。

## 管线顶点输入
现在，我们可以将填写好的顶点数据描述信息传递给管线使用。我们找到之前在createGraphicsPipeline函数中定义的vertexInputInfo结构体，使用它来引用我们填写的顶点数据描述信息：
```c++
auto bindingDescription = Vertex::getBindingDescription();
auto attributeDescriptions = Vertex::getAttributeDescriptions();

vertexInputInfo.vertexBindingDescriptionCount = 1;
vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
```
至此，我们的管线就可以接受我们自己定义的顶点数据。在启用校验层的情况下，运行程序，我们会看到没有绑定顶点缓冲这一信息，下一章节，我们会创建这个顶点缓冲，将顶点数据加载到顶点缓冲给GPU使用。