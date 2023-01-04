# Vulkan Buffer

## 创建VertexBuffer

```c++
void createVertexBuffer()
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT; //缓冲用作存储顶点数据
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //缓冲可以被特定的队列族所拥有，也可以同时在多个队列族之前共享。在这里，我们只使用了一个队列，所以选择使用独有模式。

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}

	//缓冲创建后实际还没有给它分配任何内存, vkGetBufferMemoryRequirements函数获取缓冲的内存需求
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size; //缓冲需要的内存的字节大小，它可能和bufferInfo.size的值不同
	//findMemoryType()不同类型的内存所允许进行的操作以及操作的效率有所不同。我们需要结合自己的需求选择最合适的内存类型使用
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	//函数实际分配物理内存
	if (vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}
	//分配成功就可以将分配内存和缓冲对象绑定
	vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

	//填充顶点缓冲
	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(device, vertexBufferMemory);
}
```