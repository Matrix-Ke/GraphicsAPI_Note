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

		//��Ⱦ֡�л���׼����Ⱦcommand buffer��commnad pool����ֻ��׼�������崦���������洦�� 
		m_rhi->prepareContext();

		// update per-frame buffer������CPU���ݱ��浽ȫ�ֶ����У�VP���ݣ� light��, ����Ҫװ�ص�UBO�У�
		m_render_resource->updatePerFrameBuffer(m_render_scene, m_render_camera);

		// update per-frame visible objects
		m_render_scene->updateVisibleObjects(std::static_pointer_cast<RenderResource>(m_render_resource), m_render_camera);

		// prepare pipeline's render passes data��׼��ִ����Ⱦǰ������
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

