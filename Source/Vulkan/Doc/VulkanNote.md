# vulkan 学习路线
## 博客文章
* 学习[官方推荐教程vulkan-tutorial](https://vulkan-tutorial.com/) : 本项目代码就是根据这教程实现的
* [understanding-vulkan-objects](https://gpuopen.com/learn/understanding-vulkan-objects/)
* ![vulkanDiagram图片](./Image/Vulkan-Diagram.png)


### 视频资源
* [Vulkan Game Engine Tutorial](https://youtu.be/Y9U9IE0gVHA): 讲解的非常好，非常推荐。


## vulkan核心概念理解
* 多线程部分[Multi-Threading in Vulkan](https://community.arm.com/arm-community-blogs/b/graphics-gaming-and-vr-blog/posts/multi-threading-in-vulkan)
* vulkan同步方案 [Yet another blog explaining Vulkan synchronization](https://themaister.net/blog/2019/08/14/yet-another-blog-explaining-vulkan-synchronization/)
* 提交顺序：commandBuffer——>  VkSubmitInfo   ----->  vkQueueSubmit ------> GPU 
    | 对象 | 命令提交顺序 |
    | :--- | :---- |
    | vkQueueSubmit| 多次vkqueueSubmit，提交顺序为调用顺序|
    | vkSubmitInfo|  vkQueueSubmit一次提交多个vksubmitInfo按照此数组的下标顺序 |
    | vkSubmitInfo|  填入的多个CommandBuffer，按照自己数组下标顺序 |
    | CommandBuffer | 1.不在renderPass中的命令，这些命令的提交顺序为按照在CPU上写入的顺序； 2.subpass 提交顺序按照写入顺序，但是在renderpass不存在任何提交顺序 |
* Fence 同步：
    1. Fence用于CPU和GPU之间的同步， 它有两种状态——signaled和unsignaled
    2.  vkWaitForFences会让CPU在当前位置被阻塞掉，然后一直等待到它接受的Fence变为signaled的状态
    3. 使用vkGetFenceStatus轮询
* semaphore信号量：
    1. GPU内部命令之间的同步


### vulkan名词理解： 
* **VkMemory** is just a sequence of N bytes in memory. 
* **VkImage** object adds to it e.g. information about the format (so you can address by texels, not bytes).
* **VkImageView** object helps select only part (array or mip) of the VkImage (like stringView, arrayView or whathaveyou does). Also can help to match to some incompatible interface (by type casting format).
* **VkFramebuffer** binds a VkImageView with an attachment.
* **VkRenderpass** defines which attachment will be drawn into
* [renderTarget](./Image/VulkanRenderTarget.png)
