# vulkan 学习路线
## 1.0 
* 学习[官方推荐教程vulkan-tutorial](https://vulkan-tutorial.com/) : 本项目代码就是根据这教程实现的
* [understanding-vulkan-objects](https://gpuopen.com/learn/understanding-vulkan-objects/)
* ![-图片](./Picture/Vulkan-Diagram.png)


### 视频资源
* [Vulkan Game Engine Tutorial](https://youtu.be/Y9U9IE0gVHA): 讲解的非常好，非常推荐。


## vulkan多线程理解
* [Multi-Threading in Vulkan](https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/multi-threading-in-vulkan)


### vulkan名词理解： 
* **VkMemory** is just a sequence of N bytes in memory. 
* **VkImage** object adds to it e.g. information about the format (so you can address by texels, not bytes).
* **VkImageView** object helps select only part (array or mip) of the VkImage (like stringView, arrayView or whathaveyou does). Also can help to match to some incompatible interface (by type casting format).
* **VkFramebuffer** binds a VkImageView with an attachment.
* **VkRenderpass** defines which attachment will be drawn into
* [renderTarget](./Image/VulkanRenderTarget.png)
