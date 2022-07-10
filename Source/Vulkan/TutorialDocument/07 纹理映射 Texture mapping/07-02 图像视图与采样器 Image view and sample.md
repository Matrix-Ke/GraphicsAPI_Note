# 图像视图和采样器
在本章节我们会创建两个资源来进行图像数据的采样。第一个资源我们在介绍交换链图像时已经有所了解，第二个资源和着色器采样图像数据有关，我们尚未介绍。

## 纹理图像视图
在之前的交换链图像和帧缓冲章节，我们已经介绍过访问图像需要通过图像视图进行。对于纹理图像同样需要通过图像视图来进行访问。

添加一个类成员变量来存储纹理图像的图像视图对象，编写createTextureImageView函数来创建纹理图像的图像视图对象：
```c++
VkImageView textureImageView;

		...

void initVulkan() {
		...
	createTextureImage();
	createTextureImageView();
	createVertexBuffer();
		...
}

		...

void createTextureImageView() {

}
```
createTextureImageView函数的实现可以由createImageViews函数修改得到。两者只有两处设置不同：format和image成员变量的设置。
```c++
VkImageViewCreateInfo viewInfo = {};
viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
viewInfo.image = textureImage;
viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
viewInfo.subresourceRange.baseMipLevel = 0;
viewInfo.subresourceRange.levelCount = 1;
viewInfo.subresourceRange.baseArrayLayer = 0;
viewInfo.subresourceRange.layerCount = 1;
```
在这里，由于VK_COMPONENT_SWIZZLE_IDENTITY的值实际上是0，所以我们可以省略对viewInfo.components的显式初始化，将viewInfo.components的成员变量都设置为0。最后，调用vkCreateImageView函数创建图像视图对象：
```c++
if (vkCreateImageView(device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
	throw std::runtime_error("failed to create texture image view!");
}
// 我们可以编写一个createImageView函数来简化图像视图对象的创建：
VkImageView createImageView(VkImage image, VkFormat format) {
	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}
```
现在我们可以使用createImageView函数来简化createTextureImageView的函数实现：
```c++
void createTextureImageView() {
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM);
}
// createImageViews的函数实现可以被简化为：

void createImageViews() {
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i],
							swapChainImageFormat);
	}
}
```
应用程序结束前，我们需要在清除图像对象之前清除与之关联的图像视图对象：
```c++
void cleanup() {
	cleanupSwapChain();

	vkDestroyImageView(device, textureImageView, nullptr);

	vkDestroyImage(device, textureImage, nullptr);
	vkFreeMemory(device, textureImageMemory, nullptr);
```
## 采样器
在着色器中是可以直接访问图像数据，但当图像被作为纹理时，我们通常不这样做。通常我们使用采样器来访问纹理数据，采样器可以自动地对纹理数据进行过滤和变换处理。

采样器进行的过滤操作可以很好地处理纹理采样过密的问题。考虑一个被映射到一个几何图元上的纹理，每个纹素占据了多个片段。如果我们直接采样与片段最近的纹素作为片段颜色，可能就会得到下面第一幅图的效果：
![](./../images/texture_filtering.png)

如果使用线性插值组合4个最近的纹素，我们可以得到上面第二幅图的效果。当然，有时候我们可能会喜欢第一幅图的效果(比如在编写类似我的世界这种游戏时)，当通常来说我们更多的想要第二幅图的效果。采样器可以自动地为我们进行上面这样的纹理过滤。

与之相反的是纹理采样过疏的问题，这种情况发生在多个纹素被映射到一个片段时。这会造成幻影现象，如下面第一幅图：
![](./../images/anisotropic_filtering.png)
上面第二幅图，我们可以看到远处的纹理已经变得模糊不清。解决这一问题的方法是使用采样器进行各向异性过滤,[anisotropic filtering](https://en.wikipedia.org/wiki/Anisotropic_filtering)

除了上面这些过滤器，采样器还可以进行变换操作。变换操作发生在采样超出纹理图像实际范围的数据时，下面这些图像就是使用采样器的变换操作产生的：
![](./../images/texture_addressing.png)

我们编写一个叫做createTextureSampler的函数来创建采样器对象。之后，我们会在着色器中使用创建的采样器对象采样纹理数据。
```c++
void initVulkan() {
		...
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
		...
}

		...

void createTextureSampler() {

}
```
采样器对象的配置需要填写VkSamplerCreateInfo结构体，通过它可以指定采样器使用的过滤器和变换操作。
```c++
VkSamplerCreateInfo samplerInfo = {};
samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
samplerInfo.magFilter = VK_FILTER_LINEAR;
samplerInfo.minFilter = VK_FILTER_LINEAR;
```
magFilter和minFilter成员变量用于指定纹理需要放大和缩小时使用的插值方法。纹理放大会出现采样过密的问题，纹理缩小会出现采样过疏的问题。对于这两个成员变量，设置其值为VK_FILTER_NEAREST或VK_FILTER_LINEAR，分别对应上面我们讨论的两种过滤方式。
```c++
samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
```
addressModeU、addressModeV和addressModeW用于指定寻址模式。这里的U、V、W对应X、Y和Z轴。它们的值可以是下面这些：

VK_SAMPLER_ADDRESS_MODE_REPEAT：采样超出图像范围时重复纹理
VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT：采样超出图像范围时重复镜像后的纹理
VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE：采样超出图像范围时使用距离最近的边界纹素
VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE：采样超出图像范围时使用镜像后距离最近的边界纹素
VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER：采样超出图像返回时返回设置的边界颜色
在这里，我们的采样不会超出图像范围，所以暂时不用关心使用的寻址模式。通常来说，VK_SAMPLER_ADDRESS_MODE_REPEAT模式是最常用的，可以用它来实现平铺纹理的效果。
```c++
samplerInfo.anisotropyEnable = VK_TRUE;
samplerInfo.maxAnisotropy = 16;
```
anisotropyEnable和maxAnisotropy成员变量和各向异性过滤相关。通常来说，只要性能允许，我们都会开启各向异性过滤。maxAnisotropy成员变量用于限定计算最终颜色使用的样本个数。maxAnisotropy成员变量的值越小，采样的性能表现越好，但采样结果质量较低。目前为止，还没有图形硬件能够使用超过16个样本，同时即使可以使用超过16个样本，采样效果的增强也开始变得微乎其微。
```c++
samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
// borderColor成员变量用于指定使用VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER寻址模式时采样超出图像范围时返回的边界颜色。边界颜色并非可以设置为任意颜色。它可以被设置为浮点或整型格式的黑色、白色或透明色。
samplerInfo.unnormalizedCoordinates = VK_FALSE;
// unnormalizedCoordinates成员变量用于指定采样使用的坐标系统。将其设置为VK_TRUE时，采样使用的坐标范围为[0, texWidth)和[0, texHeight)。将其设置为VK_FALSE，采样使用的坐标范围在所有轴都是[0, 1)。通常使用VK_FALSE的情况更常见，这种情况下我们可以使用相同的纹理坐标采样不同分辨率的纹理。

samplerInfo.compareEnable = VK_FALSE;
samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
// 通过compareEnable和compareOp成员变量，我们可以将样本和一个设定的值进行比较，然后将比较结果用于之后的过滤操作。通常我们在进行阴影贴图时会使用它。暂时我们不使用这一功能，在之后的章节，我们会对这一功能进行更为详细地介绍。

samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
samplerInfo.mipLodBias = 0.0f;
samplerInfo.minLod = 0.0f;
samplerInfo.maxLod = 0.0f;
```
mipmapMode、mipLodBias、minLod和maxLod成员变量用于设置分级细化(mipmap)，我们会在之后的章节对分级细化进行介绍，它可以看作是过滤操作的一种。

至此，我们就完成了VkSamplerCreateInfo结构体的填写。现在添加一个类成员变量来存储我们调用vkCreateSampler函数创建的采样器对象：
```c++
VkImageView textureImageView;
VkSampler textureSampler;

		...

void createTextureSampler() {
		...

	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}
```
需要注意，采样器对象并不引用特定的VkImage对象，它是一个用于访问纹理数据的接口。我们可以使用它来访问任意不同的图像，不管图像是一维的、二维的、还是三维的。这和一些旧的图形API将纹理图像和过滤设置绑定在一起进行采样是不同的。

最后，我们需要在应用程序结束前清除我们创建的采样器对象：
```c++
void cleanup() {
	cleanupSwapChain();

	vkDestroySampler(device, textureSampler, nullptr);
	vkDestroyImageView(device, textureImageView, nullptr);

		...
}
```
## 各向异性设备特性
如果现在读者编译运行程序，就会看到下面的校验层信息：
![](./../images/validation_layer_anisotropy.png)

这是因为各向异性过滤实际上是一个非必需的设备特性。我们需要在创建逻辑设备时检查这一设备特性是否被设备支持才能使用它：
```c++
VkPhysicalDeviceFeatures deviceFeatures = {};
deviceFeatures.samplerAnisotropy = VK_TRUE;
// 尽管，现在大部分现代图形硬件都已经支持了这一特性，但我们最好还是以防万一对设备是否支持这一特性进行检测：

bool isDeviceSuitable(VkPhysicalDevice device) {
		...

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported &&
				swapChainAdequate && supportedFeatures.samplerAnisotropy;
}
```
我们通过调用vkGetPhysicalDeviceFeatures函数获取物理设备支持的特性信息，然后验证是否包含各向异性过滤特性。

如果不想使用各向异性过滤，可以按照下面这样设置：
```c++
samplerInfo.anisotropyEnable = VK_FALSE;
samplerInfo.maxAnisotropy = 1;
```