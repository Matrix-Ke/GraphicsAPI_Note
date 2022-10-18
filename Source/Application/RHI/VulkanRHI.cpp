#include "VulkanRHI.h"

#include <cstring>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>


namespace Matrix
{
	VulkanRHI::~VulkanRHI()
	{
		// TODO
	}

	void VulkanRHI::initialize(RHIInitInfo init_info)
	{
		mWindow = init_info.window_system->getWindow();

		std::array<int, 2> window_size = init_info.window_system->getWindowSize();

		mViewport = { 0.0f, 0.0f, (float)window_size[0], (float)window_size[1], 0.0f, 1.0f };
		mScissor = { {0, 0}, {(uint32_t)window_size[0], (uint32_t)window_size[1]} };

#ifndef NDEBUG
		mEnable_validation_Layers = true;
		mEnable_debug_utils_label = true;
#else
		mEnable_validation_Layers = false;
		mEnable_debug_utils_label = false;
#endif

#if defined(__GNUC__) && defined(__MACH__)
		mEnable_point_light_shadow = false;
#else
		mEnable_point_light_shadow = true;
#endif

		createInstance();

		initializeDebugMessenger();

		createWindowSurface();

		initializePhysicalDevice();

		createLogicalDevice();

		createCommandPool();

		createCommandBuffers();

		createDescriptorPool();

		createSyncPrimitives();

		createSwapchain();

		createSwapchainImageViews();

		createFramebufferImageAndView();

		createAssetAllocator();
	}

	void VulkanRHI::prepareContext()
	{
		mPtrCurrent_frame_index = &mCurrent_frame_index;
		mCurrent_command_buffer = mCommand_buffers[mCurrent_frame_index];
		mPtrCommand_buffers = mCommand_buffers;
		mPtrCommand_pools = mCommand_pools;
	}

	void VulkanRHI::clear()
	{
		if (mEnable_validation_Layers)
		{
			destroyDebugUtilsMessengerEXT(mInstance, mDebug_messenger, nullptr);
		}
	}

	void VulkanRHI::waitForFences()
	{
		//kWaitForFences会让CPU在当前位置被阻塞掉，然后一直等待到它接受的Fence变为signaled的状态
		VkResult res_wait_for_fences = mVk_wait_for_fences(mDevice, 1, &mbFrame_in_flight_fences[mCurrent_frame_index], VK_TRUE, UINT64_MAX);
		if (VK_SUCCESS != res_wait_for_fences)
		{
			throw std::runtime_error("failed to synchronize");
		}
	}

	void VulkanRHI::resetCommandPool()
	{
		VkResult res_reset_command_pool = mVk_reset_command_pool(mDevice, mCommand_pools[mCurrent_frame_index], 0);
		if (VK_SUCCESS != res_reset_command_pool)
		{
			throw std::runtime_error("failed to synchronize");
		}
	}

	bool VulkanRHI::prepareBeforePass(std::function<void()> passUpdateAfterRecreateSwapchain)
	{
		VkResult acquire_image_result =
			vkAcquireNextImageKHR(mDevice,
				mSwapchain,
				UINT64_MAX,
				mImage_available_for_render_semaphores[mCurrent_frame_index],
				VK_NULL_HANDLE,
				&mCurrent_swapchain_image_index);

		if (VK_ERROR_OUT_OF_DATE_KHR == acquire_image_result)
		{
			recreateSwapchain();
			passUpdateAfterRecreateSwapchain();
			return true;
		}
		else if (VK_SUBOPTIMAL_KHR == acquire_image_result)
		{
			recreateSwapchain();
			passUpdateAfterRecreateSwapchain();

			// NULL submit to wait semaphore
			VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };
			VkSubmitInfo         submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = 1;
			submit_info.pWaitSemaphores = &mImage_available_for_render_semaphores[mCurrent_frame_index];
			submit_info.pWaitDstStageMask = wait_stages;
			submit_info.commandBufferCount = 0;
			submit_info.pCommandBuffers = NULL;
			submit_info.signalSemaphoreCount = 0;
			submit_info.pSignalSemaphores = NULL;

			VkResult res_reset_fences =
				mVk_reset_fences(mDevice, 1, &mbFrame_in_flight_fences[mCurrent_frame_index]);
			assert(VK_SUCCESS == res_reset_fences);

			VkResult res_queue_submit =
				vkQueueSubmit(mGraphics_queue, 1, &submit_info, mbFrame_in_flight_fences[mCurrent_frame_index]);
			assert(VK_SUCCESS == res_queue_submit);

			mCurrent_frame_index = (mCurrent_frame_index + 1) % sMaxFramesInFlight;
			return true;
		}
		else
		{
			assert(VK_SUCCESS == acquire_image_result);
		}

		// begin command buffer
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		command_buffer_begin_info.flags = 0;
		command_buffer_begin_info.pInheritanceInfo = nullptr;

		VkResult res_begin_command_buffer =
			mVk_begin_command_buffer(mCommand_buffers[mCurrent_frame_index], &command_buffer_begin_info);
		assert(VK_SUCCESS == res_begin_command_buffer);

		return false;
	}

	void VulkanRHI::submitRendering(std::function<void()> passUpdateAfterRecreateSwapchain)
	{
		// end command buffer
		VkResult res_end_command_buffer = mVk_end_command_buffer(mCommand_buffers[mCurrent_frame_index]);
		assert(VK_SUCCESS == res_end_command_buffer);

		VkSemaphore semaphores[2] = { mImage_available_for_texturescopy_semaphores[mCurrent_frame_index],
									 mImage_finished_for_presentation_semaphores[mCurrent_frame_index] };
		// submit command buffer
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo         submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &mImage_available_for_render_semaphores[mCurrent_frame_index];
		submit_info.pWaitDstStageMask = wait_stages;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &mCommand_buffers[mCurrent_frame_index];
		submit_info.signalSemaphoreCount = 2;
		submit_info.pSignalSemaphores = semaphores;

		VkResult res_reset_fences = mVk_reset_fences(mDevice, 1, &mbFrame_in_flight_fences[mCurrent_frame_index]);
		assert(VK_SUCCESS == res_reset_fences);

		VkResult res_queue_submit =
			vkQueueSubmit(mGraphics_queue, 1, &submit_info, mbFrame_in_flight_fences[mCurrent_frame_index]);
		assert(VK_SUCCESS == res_queue_submit);

		// present swapchain
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = &mImage_finished_for_presentation_semaphores[mCurrent_frame_index];
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &mSwapchain;
		present_info.pImageIndices = &mCurrent_swapchain_image_index;

		VkResult present_result = vkQueuePresentKHR(mPresent_queue, &present_info);
		if (VK_ERROR_OUT_OF_DATE_KHR == present_result || VK_SUBOPTIMAL_KHR == present_result)
		{
			recreateSwapchain();
			passUpdateAfterRecreateSwapchain();
		}
		else
		{
			assert(VK_SUCCESS == present_result);
		}

		mCurrent_frame_index = (mCurrent_frame_index + 1) % sMaxFramesInFlight;
	}

	VkCommandBuffer VulkanRHI::beginSingleTimeCommands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mCommand_pool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(mDevice, &allocInfo, &command_buffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		mVk_begin_command_buffer(command_buffer, &beginInfo);

		return command_buffer;
	}

	void VulkanRHI::endSingleTimeCommands(VkCommandBuffer command_buffer)
	{
		mVk_end_command_buffer(command_buffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &command_buffer;

		vkQueueSubmit(mGraphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mGraphics_queue);

		vkFreeCommandBuffers(mDevice, mCommand_pool, 1, &command_buffer);
	}

	// validation layers
	bool VulkanRHI::checkValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : mValidation_layers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanRHI::getRequiredExtensions()
	{
		uint32_t     glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (mEnable_validation_Layers || mEnable_debug_utils_label)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

#if defined(__MACH__)
		extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
#endif

		return extensions;
	}

	// debug callback
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void VulkanRHI::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	void VulkanRHI::createInstance()
	{
		// validation layer will be enabled in debug mode
		if (mEnable_validation_Layers && !checkValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		mVulkan_api_version = VK_API_VERSION_1_0;

		// app info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Matrix_renderer";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Matrix";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = mVulkan_api_version;

		// create info
		VkInstanceCreateInfo instance_create_info{};
		instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instance_create_info.pApplicationInfo = &appInfo; // the appInfo is stored here

		auto extensions = getRequiredExtensions();
		instance_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instance_create_info.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (mEnable_validation_Layers)
		{
			instance_create_info.enabledLayerCount = static_cast<uint32_t>(mValidation_layers.size());
			instance_create_info.ppEnabledLayerNames = mValidation_layers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			instance_create_info.enabledLayerCount = 0;
			instance_create_info.pNext = nullptr;
		}

		// create mVulkan_context._instance
		if (vkCreateInstance(&instance_create_info, nullptr, &mInstance) != VK_SUCCESS)
		{
			throw std::runtime_error("vk create instance");
		}
	}

	void VulkanRHI::initializeDebugMessenger()
	{
		if (mEnable_validation_Layers)
		{
			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			populateDebugMessengerCreateInfo(createInfo);
			if (VK_SUCCESS != createDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mDebug_messenger))
			{
				throw std::runtime_error("failed to set up debug messenger!");
			}
		}

		if (mEnable_debug_utils_label)
		{
			mVk_cmd_begin_debug_utils_label_ext =
				(PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(mInstance, "vkCmdBeginDebugUtilsLabelEXT");
			mVk_cmd_end_debug_utils_label_ext =
				(PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(mInstance, "vkCmdEndDebugUtilsLabelEXT");
		}
	}

	void VulkanRHI::createWindowSurface()
	{
		if (glfwCreateWindowSurface(mInstance, mWindow, nullptr, &mSurface) != VK_SUCCESS)
		{
			throw std::runtime_error("glfwCreateWindowSurface");
		}
	}

	void VulkanRHI::initializePhysicalDevice()
	{
		uint32_t physical_device_count;
		vkEnumeratePhysicalDevices(mInstance, &physical_device_count, nullptr);
		if (physical_device_count == 0)
		{
			throw std::runtime_error("enumerate physical devices");
		}
		else
		{
			// find one device that matches our requirement
			// or find which is the best
			std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
			vkEnumeratePhysicalDevices(mInstance, &physical_device_count, physical_devices.data());

			std::vector<std::pair<int, VkPhysicalDevice>> ranked_physical_devices;
			for (const auto& device : physical_devices)
			{
				VkPhysicalDeviceProperties physical_device_properties;
				vkGetPhysicalDeviceProperties(device, &physical_device_properties);
				int score = 0;

				if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					score += 1000;
				}
				else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					score += 100;
				}

				ranked_physical_devices.push_back({ score, device });
			}

			std::sort(ranked_physical_devices.begin(),
				ranked_physical_devices.end(),
				[](const std::pair<int, VkPhysicalDevice>& p1, const std::pair<int, VkPhysicalDevice>& p2)
				{
					return p1 > p2;
				});

			for (const auto& device : ranked_physical_devices)
			{
				if (isDeviceSuitable(device.second))
				{
					mPhysical_device = device.second;
					break;
				}
			}

			if (mPhysical_device == VK_NULL_HANDLE)
			{
				throw std::runtime_error("failed to find suitable physical device");
			}
		}
	}

	// logical device (mVulkan_context._device : graphic queue, present queue,
	// feature:samplerAnisotropy)
	void VulkanRHI::createLogicalDevice()
	{
		mQueue_indices = findQueueFamilies(mPhysical_device);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos; // all queues that need to be created
		std::set<uint32_t>                   queue_families = { mQueue_indices.mGraphics_family.value(),
											 mQueue_indices.mPresent_family.value(),
											 mQueue_indices.mCompute_family.value() };

		float queue_priority = 1.0f;
		for (uint32_t queue_family : queue_families) // for every queue family
		{
			// queue create info
			VkDeviceQueueCreateInfo queue_create_info{};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;
			queue_create_infos.push_back(queue_create_info);
		}

		// physical device features
		VkPhysicalDeviceFeatures physical_device_features = {};

		physical_device_features.samplerAnisotropy = VK_TRUE;

		// support inefficient readback storage buffer
		physical_device_features.fragmentStoresAndAtomics = VK_TRUE;

		// support independent blending
		physical_device_features.independentBlend = VK_TRUE;

		// support geometry shader
		if (mEnable_point_light_shadow)
		{
			physical_device_features.geometryShader = VK_TRUE;
		}

		// device create info
		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(mDevice_extensions.size());
		device_create_info.ppEnabledExtensionNames = mDevice_extensions.data();
		device_create_info.enabledLayerCount = 0;

		if (vkCreateDevice(mPhysical_device, &device_create_info, nullptr, &mDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("vk create device");
		}

		// initialize queues of this device
		vkGetDeviceQueue(mDevice, mQueue_indices.mGraphics_family.value(), 0, &mGraphics_queue);
		vkGetDeviceQueue(mDevice, mQueue_indices.mPresent_family.value(), 0, &mPresent_queue);
		vkGetDeviceQueue(mDevice, mQueue_indices.mCompute_family.value(), 0, &mCompute_queue);

		// more efficient pointer
		mVk_wait_for_fences = (PFN_vkWaitForFences)vkGetDeviceProcAddr(mDevice, "vkWaitForFences");
		mVk_reset_fences = (PFN_vkResetFences)vkGetDeviceProcAddr(mDevice, "vkResetFences");
		mVk_reset_command_pool = (PFN_vkResetCommandPool)vkGetDeviceProcAddr(mDevice, "vkResetCommandPool");
		mVk_begin_command_buffer = (PFN_vkBeginCommandBuffer)vkGetDeviceProcAddr(mDevice, "vkBeginCommandBuffer");
		mVk_end_command_buffer = (PFN_vkEndCommandBuffer)vkGetDeviceProcAddr(mDevice, "vkEndCommandBuffer");
		mVk_cmd_begin_render_pass = (PFN_vkCmdBeginRenderPass)vkGetDeviceProcAddr(mDevice, "vkCmdBeginRenderPass");
		mVk_cmd_next_subpass = (PFN_vkCmdNextSubpass)vkGetDeviceProcAddr(mDevice, "vkCmdNextSubpass");
		mVk_cmd_end_render_pass = (PFN_vkCmdEndRenderPass)vkGetDeviceProcAddr(mDevice, "vkCmdEndRenderPass");
		mVk_cmd_bind_pipeline = (PFN_vkCmdBindPipeline)vkGetDeviceProcAddr(mDevice, "vkCmdBindPipeline");
		mVk_cmd_set_viewport = (PFN_vkCmdSetViewport)vkGetDeviceProcAddr(mDevice, "vkCmdSetViewport");
		mVk_cmd_set_scissor = (PFN_vkCmdSetScissor)vkGetDeviceProcAddr(mDevice, "vkCmdSetScissor");
		mVk_cmd_bind_vertex_buffers =
			(PFN_vkCmdBindVertexBuffers)vkGetDeviceProcAddr(mDevice, "vkCmdBindVertexBuffers");
		mVk_cmd_bind_index_buffer = (PFN_vkCmdBindIndexBuffer)vkGetDeviceProcAddr(mDevice, "vkCmdBindIndexBuffer");
		mVk_cmd_bind_descriptor_sets =
			(PFN_vkCmdBindDescriptorSets)vkGetDeviceProcAddr(mDevice, "vkCmdBindDescriptorSets");
		mVk_cmd_draw_indexed = (PFN_vkCmdDrawIndexed)vkGetDeviceProcAddr(mDevice, "vkCmdDrawIndexed");
		mVk_cmd_clear_attachments = (PFN_vkCmdClearAttachments)vkGetDeviceProcAddr(mDevice, "vkCmdClearAttachments");

		mDepth_image_format = findDepthFormat();
	}

	void VulkanRHI::createCommandPool()
	{
		// default graphics command pool
		{
			VkCommandPoolCreateInfo command_pool_create_info{};
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.pNext = NULL;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			command_pool_create_info.queueFamilyIndex = mQueue_indices.mGraphics_family.value();

			if (vkCreateCommandPool(mDevice, &command_pool_create_info, nullptr, &mCommand_pool) != VK_SUCCESS)
			{
				throw std::runtime_error("vk create command pool");
			}
		}

		// other command pools
		{
			VkCommandPoolCreateInfo command_pool_create_info;
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.pNext = NULL;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			command_pool_create_info.queueFamilyIndex = mQueue_indices.mGraphics_family.value();

			for (uint32_t i = 0; i < sMaxFramesInFlight; ++i)
			{
				if (vkCreateCommandPool(mDevice, &command_pool_create_info, NULL, &mCommand_pools[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("vk create command pool");
				}
			}
		}
	}

	void VulkanRHI::createCommandBuffers()
	{
		VkCommandBufferAllocateInfo command_buffer_allocate_info{};
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1U;

		for (uint32_t i = 0; i < sMaxFramesInFlight; ++i)
		{
			command_buffer_allocate_info.commandPool = mCommand_pools[i];

			if (vkAllocateCommandBuffers(mDevice, &command_buffer_allocate_info, &mCommand_buffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("vk allocate command buffers");
			}
		}
	}

	void VulkanRHI::createDescriptorPool()
	{
		// Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
		// should be big enough, and thus we can sub-allocate DescriptorSet from
		// DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.

		VkDescriptorPoolSize pool_sizes[6];
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		pool_sizes[0].descriptorCount = 3 + 2 + 2 + 2 + 1 + 1 + 3 + 3;
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		pool_sizes[1].descriptorCount = 1 + 1 + 1 * mMax_vertex_blending_mesh_count;
		pool_sizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[2].descriptorCount = 1 * mMax_material_count;
		pool_sizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[3].descriptorCount = 3 + 5 * mMax_material_count + 1 + 1; // ImGui_ImplVulkan_CreateDeviceObjects
		pool_sizes[4].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		pool_sizes[4].descriptorCount = 4 + 1 + 1 + 2;
		pool_sizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		pool_sizes[5].descriptorCount = 1;

		VkDescriptorPoolCreateInfo pool_info{};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = sizeof(pool_sizes) / sizeof(pool_sizes[0]);
		pool_info.pPoolSizes = pool_sizes;
		pool_info.maxSets = 1 + 1 + 1 + mMax_material_count + mMax_vertex_blending_mesh_count + 1 + 1; // +skybox + axis descriptor set
		pool_info.flags = 0U;

		if (vkCreateDescriptorPool(mDevice, &pool_info, nullptr, &mDescriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("create descriptor pool");
		}
	}

	// semaphore : signal an image is ready for rendering // ready for presentation
	// (mVulkan_context._swapchain_images --> semaphores, fences)
	void VulkanRHI::createSyncPrimitives()
	{
		VkSemaphoreCreateInfo semaphore_create_info{};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_create_info{};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // the fence is initialized as signaled

		for (uint32_t i = 0; i < sMaxFramesInFlight; i++)
		{
			if (vkCreateSemaphore(
				mDevice, &semaphore_create_info, nullptr, &mImage_available_for_render_semaphores[i]) !=
				VK_SUCCESS ||
				vkCreateSemaphore(
					mDevice, &semaphore_create_info, nullptr, &mImage_finished_for_presentation_semaphores[i]) !=
				VK_SUCCESS ||
				vkCreateSemaphore(
					mDevice, &semaphore_create_info, nullptr, &mImage_available_for_texturescopy_semaphores[i]) !=
				VK_SUCCESS ||
				vkCreateFence(mDevice, &fence_create_info, nullptr, &mbFrame_in_flight_fences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("vk create semaphore & fence");
			}
		}
	}

	void VulkanRHI::createFramebufferImageAndView()
	{
		VulkanUtil::createImage(mPhysical_device,
			mDevice,
			mSwapchain_extent.width,
			mSwapchain_extent.height,
			mDepth_image_format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			mDepth_image,
			mDepth_image_memory,
			0,
			1,
			1);

		mDepth_image_view = VulkanUtil::createImageView(mDevice, mDepth_image, mDepth_image_format, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
	}

	void VulkanRHI::createSwapchainImageViews()
	{
		mSwapchain_imageviews.resize(mSwapchain_images.size());

		// create imageview (one for each this time) for all swapchain images
		for (size_t i = 0; i < mSwapchain_images.size(); i++)
		{
			mSwapchain_imageviews[i] = VulkanUtil::createImageView(mDevice,
				mSwapchain_images[i],
				mSwapchain_image_format,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_VIEW_TYPE_2D,
				1,
				1);
		}
	}

	void VulkanRHI::createAssetAllocator()
	{
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = mVulkan_api_version;
		allocatorCreateInfo.physicalDevice = mPhysical_device;
		allocatorCreateInfo.device = mDevice;
		allocatorCreateInfo.instance = mInstance;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

		vmaCreateAllocator(&allocatorCreateInfo, &mAssets_allocator);
	}

	void VulkanRHI::createSwapchain()
	{
		// query all supports of this physical device
		SwapChainSupportDetails swapchain_support_details = querySwapChainSupport(mPhysical_device);

		// choose the best or fitting format
		VkSurfaceFormatKHR chosen_surface_format =
			chooseSwapchainSurfaceFormatFromDetails(swapchain_support_details.mFormats);
		// choose the best or fitting present mode
		VkPresentModeKHR chosen_presentMode =
			chooseSwapchainPresentModeFromDetails(swapchain_support_details.mPresentModes);
		// choose the best or fitting extent
		VkExtent2D chosen_extent = chooseSwapchainExtentFromDetails(swapchain_support_details.mCapabilities);

		uint32_t image_count = swapchain_support_details.mCapabilities.minImageCount + 1;
		if (swapchain_support_details.mCapabilities.maxImageCount > 0 &&
			image_count > swapchain_support_details.mCapabilities.maxImageCount)
		{
			image_count = swapchain_support_details.mCapabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = mSurface;

		createInfo.minImageCount = image_count;
		createInfo.imageFormat = chosen_surface_format.format;
		createInfo.imageColorSpace = chosen_surface_format.colorSpace;
		createInfo.imageExtent = chosen_extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t queueFamilyIndices[] = { mQueue_indices.mGraphics_family.value(),
										 mQueue_indices.mPresent_family.value() };

		if (mQueue_indices.mGraphics_family != mQueue_indices.mPresent_family)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapchain_support_details.mCapabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = chosen_presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapchain) != VK_SUCCESS)
		{
			throw std::runtime_error("vk create swapchain khr");
		}

		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &image_count, nullptr);
		mSwapchain_images.resize(image_count);
		vkGetSwapchainImagesKHR(mDevice, mSwapchain, &image_count, mSwapchain_images.data());

		mSwapchain_image_format = chosen_surface_format.format;
		mSwapchain_extent = chosen_extent;

		mScissor = { {0, 0}, {mSwapchain_extent.width, mSwapchain_extent.height} };
	}

	void VulkanRHI::clearSwapchain()
	{
		for (auto imageview : mSwapchain_imageviews)
		{
			vkDestroyImageView(mDevice, imageview, NULL);
		}
		vkDestroySwapchainKHR(mDevice, mSwapchain, NULL); // also swapchain images
	}

	void VulkanRHI::recreateSwapchain()
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(mWindow, &width, &height);
		while (width == 0 || height == 0) // minimized 0,0, pause for now
		{
			glfwGetFramebufferSize(mWindow, &width, &height);
			glfwWaitEvents();
		}

		VkResult res_wait_for_fences =
			mVk_wait_for_fences(mDevice, sMaxFramesInFlight, mbFrame_in_flight_fences, VK_TRUE, UINT64_MAX);
		assert(VK_SUCCESS == res_wait_for_fences);

		vkDestroyImageView(mDevice, mDepth_image_view, NULL);
		vkDestroyImage(mDevice, mDepth_image, NULL);
		vkFreeMemory(mDevice, mDepth_image_memory, NULL);

		for (auto imageview : mSwapchain_imageviews)
		{
			vkDestroyImageView(mDevice, imageview, NULL);
		}
		vkDestroySwapchainKHR(mDevice, mSwapchain, NULL);

		createSwapchain();
		createSwapchainImageViews();
		createFramebufferImageAndView();
	}

	VkResult VulkanRHI::createDebugUtilsMessengerEXT(VkInstance                                instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func =
			(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanRHI::destroyDebugUtilsMessengerEXT(VkInstance                   instance,
		VkDebugUtilsMessengerEXT     debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func =
			(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	QueueFamilyIndices VulkanRHI::findQueueFamilies(VkPhysicalDevice physical_device) // for device and surface
	{
		QueueFamilyIndices indices;
		uint32_t           queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

		int i = 0;
		for (const auto& queue_family : queue_families)
		{
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) // if support graphics command queue
			{
				indices.mGraphics_family = i;
			}

			if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) // if support compute command queue
			{
				indices.mCompute_family = i;
			}

			VkBool32 is_present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device,
				i,
				mSurface,
				&is_present_support); // if support surface presentation
			if (is_present_support)
			{
				indices.mPresent_family = i;
			}

			if (indices.isComplete())
			{
				break;
			}
			i++;
		}
		return indices;
	}

	bool VulkanRHI::checkDeviceExtensionSupport(VkPhysicalDevice physical_device)
	{
		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

		std::set<std::string> required_extensions(mDevice_extensions.begin(), mDevice_extensions.end());
		for (const auto& extension : available_extensions)
		{
			required_extensions.erase(extension.extensionName);
		}

		return required_extensions.empty();
	}

	bool VulkanRHI::isDeviceSuitable(VkPhysicalDevice physical_device)
	{
		auto queue_indices = findQueueFamilies(physical_device);
		bool is_extensions_supported = checkDeviceExtensionSupport(physical_device);
		bool is_swapchain_adequate = false;
		if (is_extensions_supported)
		{
			SwapChainSupportDetails swapchain_support_details = querySwapChainSupport(physical_device);
			is_swapchain_adequate =
				!swapchain_support_details.mFormats.empty() && !swapchain_support_details.mPresentModes.empty();
		}

		VkPhysicalDeviceFeatures physical_device_features;
		vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

		if (!queue_indices.isComplete() || !is_swapchain_adequate || !physical_device_features.samplerAnisotropy)
		{
			return false;
		}

		return true;
	}

	SwapChainSupportDetails VulkanRHI::querySwapChainSupport(VkPhysicalDevice physical_device)
	{
		SwapChainSupportDetails details_result;

		// capabilities
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, mSurface, &details_result.mCapabilities);

		// formats
		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, mSurface, &format_count, nullptr);
		if (format_count != 0)
		{
			details_result.mFormats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(
				physical_device, mSurface, &format_count, details_result.mFormats.data());
		}

		// present modes
		uint32_t presentmode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, mSurface, &presentmode_count, nullptr);
		if (presentmode_count != 0)
		{
			details_result.mPresentModes.resize(presentmode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				physical_device, mSurface, &presentmode_count, details_result.mPresentModes.data());
		}

		return details_result;
	}

	VkFormat VulkanRHI::findDepthFormat()
	{
		return findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORmS8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

	VkFormat VulkanRHI::findSupportedFormat(const std::vector<VkFormat>& candidates,
		VkImageTiling                tiling,
		VkFormatFeatureFlags         features)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(mPhysical_device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("findSupportedFormat failed");
	}

	VkSurfaceFormatKHR
		VulkanRHI::chooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& available_surface_formats)
	{
		for (const auto& surface_format : available_surface_formats)
		{
			// TODO: select the VK_FORMAT_B8G8R8A8_SRGB surface format,
			// there is no need to do gamma correction in the fragment shader
			if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
				surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return surface_format;
			}
		}
		return available_surface_formats[0];
	}

	VkPresentModeKHR
		VulkanRHI::chooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& available_present_modes)
	{
		for (VkPresentModeKHR present_mode : available_present_modes)
		{
			if (VK_PRESENT_MODE_MAILBOX_KHR == present_mode)
			{
				return VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanRHI::chooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			int width, height;
			glfwGetFramebufferSize(mWindow, &width, &height);

			VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

			actualExtent.width =
				std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height =
				std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
}
