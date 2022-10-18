#include "RenderPipeline.h"
#include "RHI/VulkanRHI.h"
#include "Render/Pass/PointLightShadowPass.h"


namespace Matrix
{
	void RenderPipeline::passUpdateAfterRecreateSwapchain()
	{
		//更新各个pass
	}

	void RenderPipeline::forwardRender(std::shared_ptr<VulkanRHI> rhi, std::shared_ptr<RenderResourceBase> render_resource)
	{
	}
	void RenderPipeline::deferredRender(std::shared_ptr<VulkanRHI> rhi, std::shared_ptr<RenderResourceBase> render_resource)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi.get());
		RenderResource* vulkan_resource = static_cast<RenderResource*>(render_resource.get());

		vulkan_resource->resetRingBufferOffset(vulkan_rhi->mCurrent_frame_index);

		vulkan_rhi->waitForFences();

		vulkan_rhi->resetCommandPool();

		bool recreate_swapchain = vulkan_rhi->prepareBeforePass(std::bind(&RenderPipeline::passUpdateAfterRecreateSwapchain, this));
		if (recreate_swapchain)
		{
			return;
		}
		static_cast<PointLightShadowPass*>(m_point_light_shadow_pass.get())->draw();

		UIPass& ui_pass = *(static_cast<UIPass*>(m_ui_pass.get()));

		static_cast<ParticlePass*>(m_particle_pass.get())
			->setRenderCommandBufferHandle(
				static_cast<MainCameraPass*>(m_main_camera_pass.get())->getRenderCommandBuffer());

		static_cast<MainCameraPass*>(m_main_camera_pass.get())
			->draw(color_grading_pass,
				fxaa_pass,
				tone_mapping_pass,
				ui_pass,
				combine_ui_pass,
				particle_pass,
				vulkan_rhi->m_current_swapchain_image_index);

		vulkan_rhi->submitRendering(std::bind(&RenderPipeline::passUpdateAfterRecreateSwapchain, this));
		static_cast<ParticlePass*>(m_particle_pass.get())->copyNormalAndDepthImage();
		static_cast<ParticlePass*>(m_particle_pass.get())->simulate();
	}
}

