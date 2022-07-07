## 暂存缓冲
### 介绍
现在我们创建的顶点缓冲已经可以使用了，但我们的顶点缓冲使用的内存类型并不是适合显卡读取的最佳内存类型。最适合显卡读取的内存类型具有            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT标记，含有这一标记的内存类型通常CPU无法直接访问。在本章节，我们会创建两个顶点缓冲。一个用于CPU加载数据，一个用于显卡设备读取数据。我们通过缓冲复制指令将CPU加载到的缓冲中的数据复制到显卡可以快速读取的缓冲中去。

### 传输队列
缓冲复制指令需要提交给支持传输操作的队列执行，我们可以查询队列族是否支持VK_QUEUE_TRANSFER_BIT特性，确定是否可以使用缓冲复制指令。对于支持VK_QUEUE_GRAPHICS_BIT或VK_QUEUE_COMPUTE_BIT特性的队列族，VK_QUEUE_TRANSFER_BIT特性一定被支持，所以我们不需要显式地检测队列族是否支持VK_QUEUE_TRANSFER_BIT特性。

如果读者喜欢挑战，可以尝试使用其它支持传输操作的队列族。这需要读者按照下面对程序进行一定地修改：

修改QueueFamilyIndices和findQueueFamilies显式地查找具有VK_QUEUE_TRANSFER_BIT特性，但不具有VK_QUEUE_GRAPHICS_BIT特性的队列族。
修改createLogicalDevice函数，申请一个传输队列。
创建另外一个指令池对象用于分配用于传输队列的指令缓冲对象。
修改sharingMode为VK_SHARING_MODE_CONCURRENT。指定使用图形和传输两个队列族。
提交传输指令(比如vkCmdCopyBuffer)给传输队列而不是之前的图形队列。
看起来需要修改的地方还比较多，但复杂的修改也更容易让我们对资源在队列族之间的共享有更深刻的理解。

### 创建抽象缓冲
我们添加一个叫做createBuffer的辅助函数来帮助我们创建缓冲：

```c++
void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex =
	findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
```
我们添加了一些参数来方便地使用不同的缓冲大小，内存类型来创建我们需要的缓冲。createBuffer函数的最后两个参数用于返回创建的缓冲对象和它关联的内存对象。
现在我们可以使用createBuffer函数替换之前createVertexBuffer函数的实现内容：

```c++
void createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer,
				vertexBufferMemory);

	void* data;
	vkMapMemory(device, vertexBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(device, vertexBufferMemory);
}
```
编译运行程序，确保顶点缓冲工作正常。

使用暂存缓冲
修改createVertexBuffer函数，使用CPU可见的缓冲作为临时缓冲，使用显卡读取较快的缓冲作为真正的顶点缓冲：
```c++
void createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
				stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t) bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer,
				vertexBufferMemory);
}
```
现在我们可以使用新的关联stagingBufferMemory作为内存的stagingBuffer缓冲对象来存放CPU加载的顶点数据。在本章节，我们会使用下面两个缓冲使用标记：

VK_BUFFER_USAGE_TRANSFER_SRC_BIT：缓冲可以被用作内存传输操作的数据来源。
VK_BUFFER_USAGE_TRANSFER_DST_BIT：缓冲可以被用作内存传输操作的目的缓冲。
vertexBuffer现在关联的内存是设备所有的，不能vkMapMemory函数对它关联的内存进行映射。我们只能通过stagingBuffer来向vertexBuffer复制数据。我们需要使用标记指明我们使用缓冲进行传输操作。

添加一个copyBuffer的辅助函数用于在缓冲之间复制数据：
```c++
void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

}
我们需要一个支持内存传输指令的指令缓冲来记录内存传输指令，然后提交到内存传输指令队列执行内存传输。通常，我们会为内存传输指令使用的指令缓冲创建另外的指令池对象，这是因为内存传输指令的指令缓存通常生命周期很短，为它们使用独立的指令池对象，可以进行更好的优化。我们可以在创建指令池对象时为它指定VK_COMMAND_POOL_CREATE_TRANSIENT_BIT标记。

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
}
```
开始记录内存传输指令：
```c++
VkCommandBufferBeginInfo beginInfo = {};
beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
vkBeginCommandBuffer(commandBuffer, &beginInfo);
```
我们之前对绘制指令缓冲使用的VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT标记，在这里不是必须的，这是因为我们只使用这个指令缓冲一次，等待复制操作完成后才继续程序的执行。我们可以使用VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT标记告诉驱动程序我们如何使用这个指令缓冲，来让驱动程序进行更好的优化。
```c++
VkBufferCopy copyRegion = {};
copyRegion.srcOffset = 0; // Optional
copyRegion.dstOffset = 0; // Optional
copyRegion.size = size;
vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
```
我们使用vkCmdCopyBuffer指令来进行缓冲的复制操作。它以源缓冲对象和目的缓冲对象，以及一个VkBufferCopy数组为参数。VkBufferCopy数组指定了复制操作的源缓冲位置偏移，目的缓冲位置偏移，以及要复制的数据长度。和vkMapMemory指令不同，这里不能使用VK_WHOLE_SIZE来指定要复制的数据长度。
```c++
vkEndCommandBuffer(commandBuffer);
我们的内存传输操作使用的指令缓冲只包含了复制指令，记录完复制指令后，我们就可以结束指令缓冲的记录操作，提交指令缓冲完成传输操作的执行：

VkSubmitInfo submitInfo = {};
submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
submitInfo.commandBufferCount = 1;
submitInfo.pCommandBuffers = &commandBuffer;

vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
vkQueueWaitIdle(graphicsQueue);
```
和绘制指令不同，这一次我们直接等待传输操作完成。有两种等待内存传输操作完成的方法：一种是使用栅栏(fence)，通过vkWaitForFences函数等待。另一种是通过vkQueueWaitIdle函数等待。使用栅栏(fence)可以同步多个不同的内存传输操作，给驱动程序的优化空间也更大。
```c++
vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
```
最后，传输操作完成后我们需要清除我们使用的指令缓冲对象。
接着，我们可以在createVertexBuffer函数中调用copyBuffer函数复制顶点数据到显卡读取较快的缓冲中：
```c++
createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer,
			vertexBufferMemory);

copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
最后，不要忘记清除我们使用的缓冲对象和它关联的内存对象：

		...

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}
```
编译运行程序，确保一切正常。暂时由于我们使用的顶点数据过于简单，性能提升并不明显。当我们渲染更为复杂的对象时，可以看到更为明显的提升。

### 总结
实际上，很少有程序为每个缓冲对象都调用vkAllocateMemory函数分配关联内存。物体设备允许同时存在的内存分配次数是有限制的，它最大为maxMemoryAllocationCount。即使在高端硬件上，比如NVIDIA GTX 1080，maxMemoryAllocationCount也只有4096这么大。所以，通常我们会一次申请一个很大块的内存，然后基于这个内存实现自己的内存分配器为我们创建的对象通过偏移参数分配内存。

我们可以自己实现内存分配器，也可以使用GPUOpen提供的VulkanMemoryAllocator内存分配器。在本教程，我们的内存分配次数实际上很小，所以我们为每个需要内存的对象调用vkAllocateMemory函数分配内存。