#pragma once
#include "Render/RenderPass.h"

#include <memory>
#include <vector>


namespace Matrix
{
	struct RenderPipelineInitInfo
	{
		bool                                enable_fxaa{ false };
		std::shared_ptr<RenderResourceBase> render_resource;
	};
	/*
	renderpipeline 1. 负责初始化各个pass  2.控制各个pass调用顺序
	*/

	class RenderPipeline
	{
	public:
		friend class RenderSystem;

	public:
		virtual ~RenderPipeline() {}

		virtual void initialize(RenderPipelineInitInfo init_info) = 0;
		void passUpdateAfterRecreateSwapchain();

		virtual void preparePassData(std::shared_ptr<RenderResourceBase> render_resource);
		virtual void forwardRender(std::shared_ptr<VulkanRHI> rhi, std::shared_ptr<RenderResourceBase> render_resource);
		virtual void deferredRender(std::shared_ptr<VulkanRHI> rhi, std::shared_ptr<RenderResourceBase> render_resource);

		void		initializeUIRenderBackend(EditorUI* window_ui);

	protected:
		std::shared_ptr<VulkanRHI> m_rhi;

		std::shared_ptr<RenderPassBase> m_directional_light_pass;
		std::shared_ptr<RenderPassBase> m_point_light_shadow_pass;
		std::shared_ptr<RenderPassBase> m_main_camera_pass;
		std::shared_ptr<RenderPassBase> m_color_grading_pass;
		std::shared_ptr<RenderPassBase> m_fxaa_pass;
		std::shared_ptr<RenderPassBase> m_tone_mapping_pass;
		std::shared_ptr<RenderPassBase> m_ui_pass;
		std::shared_ptr<RenderPassBase> m_combine_ui_pass;
		std::shared_ptr<RenderPassBase> m_pick_pass;
		std::shared_ptr<RenderPassBase> m_particle_pass;

	};
}