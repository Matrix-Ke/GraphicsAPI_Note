#pragma once

#include "Editor/WindowsApplication.h"
#include "Render/RenderType.h"

#include <array>
#include <memory>
#include <optional>

namespace Matrix
{
	class WindowSystem;
	class VulkanRHI;
	class RenderResource;
	class RenderPipeline;
	class RenderScene;
	class RenderCamera;

	struct RenderSystemInitInfo
	{
		std::shared_ptr<WindowSystem> window_system;
	};

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem();

		void initialize(RenderSystemInitInfo init_info);
		void tick();

		void processSwapData(); //处理CPU准备好的下一帧逻辑
	private:
		RENDER_PIPELINE_TYPE m_render_pipeline_type{ RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE };
		//RenderSwapContext m_swap_context;

		std::shared_ptr<VulkanRHI>          m_rhi;
		std::shared_ptr<RenderCamera>       m_render_camera;
		std::shared_ptr<RenderScene>        m_render_scene;
		std::shared_ptr<RenderResource>		m_render_resource;
		std::shared_ptr<RenderPipeline>		m_render_pipeline;
	};

}

