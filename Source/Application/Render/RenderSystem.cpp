#include "RenderSystem.h"

#include "RHI/VulkanRHI.h"
#include "Render/RenderResource.h"
#include "Render/RenderScene.h"
#include "Render/RenderPipeline.h"

#include "Global/Global_context.h"
#include "Core/logSysytem.h"


namespace Matrix
{
	void RenderSystem::tick()
	{

		// process swap data between logic and render contexts
		processSwapData();

		//渲染帧切换，准备渲染command buffer和commnad pool。（只是准备，具体处理留待后面处理） 
		m_rhi->prepareContext();

		// update per-frame buffer（）将CPU数据保存到全局对象中（VP数据， light等, 将来要装载到UBO中）
		m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);

		// update per-frame visible objects
		m_render_scene->updateVisibleObjects(std::static_pointer_cast<RenderResource>(m_render_resource), m_render_camera);

		// prepare pipeline's render passes data，准备执行渲染前的数据
		m_render_pipeline->preparePassData(m_render_resource);

		// render one frame
		if (m_render_pipeline_type == RENDER_PIPELINE_TYPE::FORWARD_PIPELINE)
		{
			m_render_pipeline->forwardRender(m_rhi, m_render_resource);
		}
		else if (m_render_pipeline_type == RENDER_PIPELINE_TYPE::DEFERRED_PIPELINE)
		{
			m_render_pipeline->deferredRender(m_rhi, m_render_resource);
		}
		else
		{
			LOG_ERROR(__FUNCTION__, "unsupported render pipeline type");
		}
	}

	void RenderSystem::processSwapData()
	{
	}
}

