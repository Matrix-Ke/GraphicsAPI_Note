# 描述符池和描述符集
## 介绍
上一章节我们介绍了用于描述可以绑定的描述符类型的描述符布局。在这一章节，我们为每一个vkBuffer创建描述符集，将其和uniform缓冲描述符进行绑定。

## 描述符池
描述符集不能被直接创建，需要通过描述符池来分配。我们添加一个叫做createDescriptorPool的函数来进行描述符池的创建：
```c++
void initVulkan() {
		...
	createUniformBuffer();
	createDescriptorPool();
		...
}

		...

void createDescriptorPool() {

}
```

我们通过VkDescriptorPoolSize结构体来对描述符池可以分配的描述符集进行定义：
```c++
VkDescriptorPoolSize poolSize = {};
poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
poolSize.descriptorCount = static_cast<uint32_t>(swapChainImages.size());
```
我们会在每一帧分配一个描述符。描述符池的大小需要通过VkDescriptorPoolCreateInfo结构体定义：
```c++
VkDescriptorPoolCreateInfo poolInfo = {};
poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
poolInfo.poolSizeCount = 1;
poolInfo.pPoolSizes = &poolSize;
```
除了可用的最大独立描述符个数外，我们还需要指定可以分配的最大描述符集个数：
```c++
poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
```
VkDescriptorPoolCreateInfo结构体有一个用于优化的标记，它决定了独立的描述符集是否可以被清除掉：VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT。由于我们在描述符集创建后，就不再对其进行操作，所以我们不需要使用这一标记，将其设置为0来使用它的默认值。
```c++
VkDescriptorPool descriptorPool;

		...

if (vkCreateDescriptorPool(device, &poolInfo, nullptr,
				&descriptorPool) != VK_SUCCESS) {
	throw std::runtime_error("failed to create descriptor pool!");
}
我们添加了一个新的类成员变量来存储使用vkCreateDescriptorPool函数创建的描述符池对象。应用程序退出前，我们需要清除我们创建的描述符池对象：

void cleanup() {
	cleanupSwapChain();

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);

		...
}
```
## 描述符集
有了描述符池，现在我们可以开始创建描述符集对象了。添加一个叫做createDescriptorSets的函数来完成描述符集对象的创建：
```c++
void initVulkan() {
		...
	createDescriptorPool();
	createDescriptorSets();
		...
}

		...

void createDescriptorSets() {

}
```
描述符集的创建需要我们填写VkDescriptorSetAllocateInfo结构体。我们需要指定分配描述符集对象的描述符池，需要分配的描述符集数量，以及它们使用的描述符布局：
```c++
std::vector<VkDescriptorSetLayout>
layouts(swapChainImages.size(), descriptorSetLayout);
VkDescriptorSetAllocateInfo allocInfo = {};
allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
allocInfo.descriptorPool = descriptorPool;
allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
allocInfo.pSetLayouts = layouts.data();
```
在这里，我们为每一个交换链图像使用相同的描述符布局创建对应的描述符集。但由于描述符布局对象个数要匹配描述符集对象个数，所以，我们还是需要使用多个相同的描述符布局对象。
添加类成员变量来存储使用vkAllocateDescriptorSets函数创建的描述符集对象：
```c++
VkDescriptorPool descriptorPool;
std::vector<VkDescriptorSet> descriptorSets;

		...

descriptorSets.resize(swapChainImages.size());
if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSets[0]) != VK_SUCCESS) {
	throw std::runtime_error("failed to allocate descriptor sets!");
}
```
描述符集对象会在描述符池对象清除时自动被清除，所以不需要我们自己显式地清除描述符对象。vkAllocateDescriptorSets函数分配地描述符集对象，每一个都带有一个uniform缓冲描述符。
描述符集对象创建后，还需要进行一定地配置。我们使用循环来遍历描述符集对象，对它进行配置：
```c++
for (size_t i = 0; i < swapChainImages.size(); i++) {

}
```
我们通过VkDescriptorBufferInfo结构体来配置描述符引用的缓冲对象。VkDescriptorBufferInfo结构体可以指定缓冲对象和可以访问的数据范围：
```c++
for (size_t i = 0; i < swapChainImages.size(); i++) {
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffers[i];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(UniformBufferObject);
}
```
如果读者需要使用整个缓冲，可以将range成员变量的值设置为VK_WHOLE_SIZE。更新描述符的配置可以使用vkUpdateDescriptorSets函数进行，它以VkWriteDescriptorSet结构体数组作为参数：
```c++
VkWriteDescriptorSet descriptorWrite = {};
descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
descriptorWrite.dstSet = descriptorSets[i];
descriptorWrite.dstBinding = 0;
descriptorWrite.dstArrayElement = 0;
```
dstSet和dstBinding成员变量用于指定要更新的描述符集对象以及缓冲绑定。在这里，我们将uniform缓冲绑定到索引0。需要注意描述符可以是数组，所以我们还需要指定数组的第一个元素的索引，在这里，我们没有使用数组作为描述符，将索引指定为0即可。
```c++
descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
descriptorWrite.descriptorCount = 1;
// 我们需要再次指定描述符的类型。可以通过设置dstArrayElement和descriptorCount成员变量一次更新多个描述符。
descriptorWrite.pBufferInfo = &bufferInfo;
descriptorWrite.pImageInfo = nullptr; // Optional
descriptorWrite.pTexelBufferView = nullptr; // Optional
```
pBufferInfo成员变量用于指定描述符引用的缓冲数据。pImageInfo成员变量用于指定描述符引用的图像数据。pTexelBufferView成员变量用于指定描述符引用的缓冲视图。这里，我们的描述符使用来访问缓冲的，所以我们只使用pBufferInfo成员变量。
```c++
vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
```
vkUpdateDescriptorSets函数可以接受两个数组作为参数：VkWriteDescriptorSet结构体数组和VkCopyDescriptorSet结构体数组。后者被用来复制描述符对象。

## 使用描述符集
现在修改createCommandBuffers函数为每个交换链图像绑定对应的描述符集。这需要调用cmdBindDescriptorSets函数来完成，我们在调用vkCmdDrawIndexed函数之前调用这一函数：
```c++
vkCmdBindDescriptorSets(commandBuffers[i],
VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
```
和顶点缓冲、索引缓冲不同，描述符集并不是图形管线所独有的，所以需要我们指定我们要绑定的是图形管线还是计算管线。管线类型之后的参数是描述符所使用的布局。接着三个参数用于指定描述符集的第一个元素索引，需要绑定的描述符集个数，以及用于绑定的描述符集数组。最后两个参数用于指定动态描述符的数组偏移。在之后的章节，我们会对它们进行更为详细地讨论。

现在编译运行程序，读者会发现窗口一片漆黑，看不到我们渲染的矩形了。这是因为之前我们设置的投影矩阵将Y轴反转，导致顶点按照顺时针绘制构成背面，从而被背面剔除。我们可以修改createGraphicsPipeline函数中VkPipelineRasterizationStateCreateInfo结构体的frontFace成员变量对于正面的设置来解决这一问题：
```c++
rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
```
重新编译运行程序，就可以看到下面的画面：
![](./../images/spinning_quad.png)

之前的矩形，现在变成了一个正方形，这是我们设置的投影矩阵的宽高比导致的。updateUniformBuffer函数在每一帧被调用，可以响应窗口大小变化，不需要在recreateSwapChain函数中对描述符进行重建。

## 多个描述符集
实际上，如我们之前看到的这些函数，可以同时绑定多个描述符集。我们只需要在创建管线布局时为每个描述符集指定一个描述符布局即可。着色器就可以像下面这样引用特定的描述符集：

layout(set = 0, binding = 0) uniform UniformBufferObject { ... }
我们可以使用这一特性将共享的描述符放入独立的描述符集中。这样就可以避免在多个不同的绘制调用重新绑定描述符从而提高性能表现。