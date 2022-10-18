#include "RenderPass.h"

namespace Matrix
{
	VisiableNodes RenderPass::m_visiable_nodes;

	std::vector<VkImageView> RenderPass::getFramebufferImageViews() const
	{
		std::vector<VkImageView> image_views;
		for (auto& attach : m_framebuffer.attachments)
		{
			image_views.push_back(attach.view);
		}
		return image_views;
	}
	std::vector<VkDescriptorSetLayout> RenderPass::getDescriptorSetLayouts() const
	{
		std::vector<VkDescriptorSetLayout> layouts;
		for (auto& desc : m_descriptor_infos)
		{
			layouts.push_back(desc.layout);
		}
		return layouts;
	}
}
