#include "UIPass.h"
#include "RHI/VulkanRHI.h"
#include "Core/ConfigManager.h"
#include "Editor/EditorUI.h"
#include "Editor/WindowsApplication.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <cassert>

namespace Matrix
{
	void UIPass::initialize(const RenderPassInitInfo* init_info)
	{
		RenderPass::initialize(nullptr);

		m_framebuffer.render_pass = static_cast<const UIPassInitInfo*>(init_info)->render_pass;
	}

	void UIPass::initializeUIRenderBackend(EditorUI* window_ui)
	{
		m_window_ui = window_ui;

		ImGui_ImplGlfw_InitForVulkan(m_vulkan_rhi->mWindow, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = m_vulkan_rhi->mInstance;
		init_info.PhysicalDevice = m_vulkan_rhi->mPhysical_device;
		init_info.Device = m_vulkan_rhi->mDevice;
		init_info.QueueFamily = m_vulkan_rhi->mQueue_indices.mGraphics_family.value();
		init_info.Queue = m_vulkan_rhi->mGraphics_queue;
		init_info.DescriptorPool = m_vulkan_rhi->mDescriptor_pool;
		init_info.Subpass = _main_camera_subpass_ui;

		// may be different from the real swapchain image count
		// see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
		init_info.MinImageCount = 3;
		init_info.ImageCount = 3;
		ImGui_ImplVulkan_Init(&init_info, m_framebuffer.render_pass);

		uploadFonts();
	}

	void UIPass::uploadFonts()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_vulkan_rhi->mCommand_pool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer = {};
		if (VK_SUCCESS != vkAllocateCommandBuffers(m_vulkan_rhi->mDevice, &allocInfo, &commandBuffer))
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (VK_SUCCESS != vkBeginCommandBuffer(commandBuffer, &beginInfo))
		{
			throw std::runtime_error("Could not create one-time command buffer!");
		}

		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

		if (VK_SUCCESS != vkEndCommandBuffer(commandBuffer))
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_vulkan_rhi->mGraphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_vulkan_rhi->mGraphics_queue);

		vkFreeCommandBuffers(m_vulkan_rhi->mDevice, m_vulkan_rhi->mCommand_pool, 1, &commandBuffer);

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void UIPass::draw()
	{
		if (m_window_ui)
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			m_window_ui->preRender();


			ImGui::Render();

			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_vulkan_rhi->mCurrent_command_buffer);

		}
	}
} 
