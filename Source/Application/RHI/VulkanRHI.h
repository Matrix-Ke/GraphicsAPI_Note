//参考此工程下Vulkan tutorial中的实现。
#include "Editor/Application.h"

#include <functional>
#include <optional>
#include <vector>


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Matrix
{
	class WindowSystem;

	struct RHIInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
	};


	struct QueueFamilyIndices
	{
		std::optional<uint32_t> mGraphics_family; //绘制
		std::optional<uint32_t> mPresent_family;// 显示
		std::optional<uint32_t> mCompute_family; //计算

		bool isComplete()
		{
			return mGraphics_family.has_value() && mPresent_family.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR		mCapabilities;
		std::vector<VkSurfaceFormatKHR> mFormats;
		std::vector<VkPresentModeKHR>	mPresentModes;
	};



	//此处可以继承基类以实现多平台图形api
	class VulkanRHI
	{
	public:
		VulkanRHI();
		~VulkanRHI();
		//base function
		void initialize(RHIInitInfo init_info);
		void prepareContext();
		//renderpass and pipeline
		bool prepareBeforePass(std::function<void()> passUpdateAfterRecreateSwapchain);
		void submitRendering(std::function<void()> passUpdateAfterRecreateSwapchain);

	public:
		bool         isPointLightShadowEnabled() const { return mEnable_point_light_shadow; }
		bool         isValidationLayerEnabled() const { return mEnable_validation_Layers; }

	public:
		//instance
		void createInstance();
		void createAssetAllocator();

		//Device
		void initializePhysicalDevice(); //选择一个支持所需功能的显卡
		void createLogicalDevice();

		//windows surface
		void createWindowSurface();

		//swapchain
		void createSwapchain();
		void clearSwapchain();
		void recreateSwapchain();

		//imageView
		void createSwapchainImageViews();
		void createFramebufferImageAndView();

		//Command
		void createCommandPool();
		void createCommandBuffers();
		void resetCommandPool();
		void endSingleTimeCommands(VkCommandBuffer command_buffer);
		VkCommandBuffer beginSingleTimeCommands();
		//synchronize
		void waitForFences();
		void createSyncPrimitives();

		//descriptor
		void createDescriptorPool();

		// clear
		void clear();

	private:
		//Layer&Extension: 辅助函数
		bool                     checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();
		void                     populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult				 createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void					 destroyDebugUtilsMessengerEXT(VkInstance  instance, VkDebugUtilsMessengerEXT  debugMessenger, const VkAllocationCallbacks* pAllocator);
		void initializeDebugMessenger();

		//device : 基本设备可用性检查, 查找支持把图像显示到表面上的队列家族, 检查需要的扩展
		bool					 checkDeviceExtensionSupport(VkPhysicalDevice physical_device);
		bool					 isDeviceSuitable(VkPhysicalDevice physical_device);
		QueueFamilyIndices		 findQueueFamilies(VkPhysicalDevice physical_device);

		//SwapChain : 仅仅是查询交换链是否可用还不够，因为它有可能与我们的表面不兼容。创建交换链同样涉及到许多设置项目，而且比创建实例和设备的设置项目多得多
		SwapChainSupportDetails	 querySwapChainSupport(VkPhysicalDevice physical_device);
		VkSurfaceFormatKHR		 chooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& available_surface_formats);
		VkPresentModeKHR		 chooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D				 chooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities);
		//depth： 查找即符合需求又被设备支持的深度图形数据格式。
		VkFormat				 findDepthFormat(); //attachment
		VkFormat				 findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling  tiling, VkFormatFeatureFlags features);

	public:
		// function pointers
		PFN_vkWaitForFences						mVk_wait_for_fences;
		PFN_vkResetFences						mVk_reset_fences;
		PFN_vkResetCommandPool					mVk_reset_command_pool;
		PFN_vkBeginCommandBuffer				mVk_begin_command_buffer;
		PFN_vkEndCommandBuffer					mVk_end_command_buffer;
		PFN_vkCmdBeginRenderPass				mVk_cmd_begin_render_pass;
		PFN_vkCmdNextSubpass					mVk_cmd_next_subpass;
		PFN_vkCmdEndRenderPass					mVk_cmd_end_render_pass;
		PFN_vkCmdBindPipeline					mVk_cmd_bind_pipeline;
		PFN_vkCmdSetViewport					mVk_cmd_set_viewport;
		PFN_vkCmdSetScissor						mVk_cmd_set_scissor;
		PFN_vkCmdBindVertexBuffers				mVk_cmd_bind_vertex_buffers;
		PFN_vkCmdBindIndexBuffer				mVk_cmd_bind_index_buffer;
		PFN_vkCmdBindDescriptorSets				mVk_cmd_bind_descriptor_sets;
		PFN_vkCmdDrawIndexed					mVk_cmd_draw_indexed;
		PFN_vkCmdClearAttachments				mVk_cmd_clear_attachments;
		// debug utilities label
		PFN_vkCmdBeginDebugUtilsLabelEXT		mVk_cmd_begin_debug_utils_label_ext;
		PFN_vkCmdEndDebugUtilsLabelEXT			mVk_cmd_end_debug_utils_label_ext;

	public:
		//device
		GLFWwindow* mWindow{ nullptr };
		// asset allocator use VMA library
		VmaAllocator					mAssets_allocator;
		VkInstance						mInstance{ VK_NULL_HANDLE };
		VkSurfaceKHR					mSurface{ VK_NULL_HANDLE };
		VkPhysicalDevice				mPhysical_device{ VK_NULL_HANDLE };
		QueueFamilyIndices				mQueue_indices;
		VkDevice						mDevice{ VK_NULL_HANDLE };

		//swapchain
		VkSwapchainKHR					mSwapchain{ VK_NULL_HANDLE };
		VkFormat						mSwapchain_image_format{ VK_FORMAT_UNDEFINED };
		VkExtent2D						mSwapchain_extent;
		std::vector<VkImage>			mSwapchain_images;
		std::vector<VkImageView>		mSwapchain_imageviews;
		std::vector<VkFramebuffer>		mSwapchain_framebuffers;
		VkViewport						mViewport;
		VkRect2D						mScissor;
		uint32_t						mCurrent_swapchain_image_index;

		//depth
		VkFormat						mDepth_image_format{ VK_FORMAT_UNDEFINED };
		VkImage							mDepth_image{ VK_NULL_HANDLE };
		VkDeviceMemory					mDepth_image_memory{ VK_NULL_HANDLE };
		VkImageView						mDepth_image_view{ VK_NULL_HANDLE };

		//queue
		VkQueue							mGraphics_queue{ VK_NULL_HANDLE };
		VkQueue							mPresent_queue{ VK_NULL_HANDLE };
		VkQueue							mCompute_queue{ VK_NULL_HANDLE };
		VkCommandPool					mCommand_pool{ VK_NULL_HANDLE };

		//global descriptor pool
		VkDescriptorPool				mDescriptor_pool;

		//command pool and buffers
		static uint8_t const			sMaxFramesInFlight{ 3 };
		uint8_t							mCurrent_frame_index{ 0 };
		VkCommandPool					mCommand_pools[sMaxFramesInFlight];
		VkCommandBuffer					mCommand_buffers[sMaxFramesInFlight];
		VkSemaphore						mImage_available_for_render_semaphores[sMaxFramesInFlight];
		VkSemaphore						mImage_available_for_texturescopy_semaphores[sMaxFramesInFlight];
		VkSemaphore						mImage_finished_for_presentation_semaphores[sMaxFramesInFlight];
		VkFence							mbFrame_in_flight_fences[sMaxFramesInFlight];
		VkCommandBuffer					mCurrent_command_buffer;
		uint8_t* mPtrCurrent_frame_index{ nullptr };
		VkCommandPool* mPtrCommand_pools{ nullptr };
		VkCommandBuffer* mPtrCommand_buffers{ nullptr };

	private:
		const std::vector<char const*>	mValidation_layers{ "VK_LAYER_KHRONOS_validation" };
		uint32_t						mVulkan_api_version{ VK_API_VERSION_1_0 };
		std::vector<char const*>		mDevice_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkDebugUtilsMessengerEXT		mDebug_messenger{ VK_NULL_HANDLE };

	private:
		//Vulkan全局设置
		bool mEnable_validation_Layers{ true };
		bool mEnable_debug_utils_label{ true };
		bool mEnable_point_light_shadow{ true };

		//used in descriptor pool creation
		uint32_t mMax_vertex_blending_mesh_count{ 256 };
		uint32_t mMax_material_count{ 256 };
	};
}