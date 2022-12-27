#include "RenderSystem.h"

#include "RHI/VulkanRHI.h"
#include "Render/RenderResource.h"
#include "Render/RenderScene.h"
#include "Render/RenderPipeline.h"

#include "Core/GlobalContext.h"
#include "Core/logSysytem.h"


namespace Matrix
{
	RenderSystem::~RenderSystem()
	{

	}

	void RenderSystem::initialize(RenderSystemInitInfo init_info)
	{
		std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
		assert(config_manager);
		//std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
		//assert(asset_manager);

		// render context initialize
		RHIInitInfo rhi_init_info;
		rhi_init_info.window_system = init_info.window_system;

		m_rhi = std::make_shared<VulkanRHI>();
		m_rhi->initialize(rhi_init_info);

		//global rendering resource
		//	GlobalRenderingRes global_rendering_res;
		//const std::string& global_rendering_res_url = config_manager->getGlobalRenderingResUrl();
		//asset_manager->loadAsset(global_rendering_res_url, global_rendering_res);

		// upload ibl, color grading textures
		LevelResourceDesc level_resource_desc;
		level_resource_desc.m_ibl_resource_desc.m_skybox_irradiance_map = global_rendering_res.m_skybox_irradiance_map;
		level_resource_desc.m_ibl_resource_desc.m_skybox_specular_map = global_rendering_res.m_skybox_specular_map;
		level_resource_desc.m_ibl_resource_desc.m_brdf_map = global_rendering_res.m_brdf_map;
		level_resource_desc.m_color_grading_resource_desc.m_color_grading_map = global_rendering_res.m_color_grading_map;

		m_render_resource = std::make_shared<RenderResource>();
		m_render_resource->uploadGlobalRenderResource(m_rhi, level_resource_desc);

		// setup render camera
		const CameraPose& camera_pose = global_rendering_res.m_camera_config.m_pose;
		m_render_camera = std::make_shared<RenderCamera>();
		m_render_camera->lookAt(camera_pose.m_position, camera_pose.m_target, camera_pose.m_up);
		m_render_camera->m_zfar = global_rendering_res.m_camera_config.m_z_far;
		m_render_camera->m_znear = global_rendering_res.m_camera_config.m_z_near;
		m_render_camera->setAspect(global_rendering_res.m_camera_config.m_aspect.x /
			global_rendering_res.m_camera_config.m_aspect.y);

		// setup render scene
		m_render_scene = std::make_shared<RenderScene>();
		m_render_scene->m_ambient_light = { global_rendering_res.m_ambient_light.toVector3() };
		m_render_scene->m_directional_light.m_direction =
			global_rendering_res.m_directional_light.m_direction.normalisedCopy();
		m_render_scene->m_directional_light.m_color = global_rendering_res.m_directional_light.m_color.toVector3();
		m_render_scene->setVisibleNodesReference();

		// initialize render pipeline
		RenderPipelineInitInfo pipeline_init_info;
		pipeline_init_info.enable_fxaa = global_rendering_res.m_enable_fxaa;
		pipeline_init_info.render_resource = m_render_resource;

		m_render_pipeline = std::make_shared<RenderPipeline>();
		m_render_pipeline->m_rhi = m_rhi;
		m_render_pipeline->initialize(pipeline_init_info);

		// descriptor set layout in main camera pass will be used when uploading resource
		std::static_pointer_cast<RenderResource>(m_render_resource)->m_mesh_descriptor_set_layout =
			&static_cast<RenderPass*>(m_render_pipeline->m_main_camera_pass.get())
			->m_descriptor_infos[MainCameraPass::LayoutType::_per_mesh]
			.layout;
		std::static_pointer_cast<RenderResource>(m_render_resource)->m_material_descriptor_set_layout =
			&static_cast<RenderPass*>(m_render_pipeline->m_main_camera_pass.get())
			->m_descriptor_infos[MainCameraPass::LayoutType::_mesh_per_material].layout;
	}

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

	void RenderSystem::swapLogicRenderData() { m_swap_context.swapLogicRenderData(); }

	RenderSwapContext& RenderSystem::getSwapContext() { return m_swap_context; }

	std::shared_ptr<RenderCamera> RenderSystem::getRenderCamera() const { return m_render_camera; }

	void RenderSystem::initializeUIRenderBackend(EditorUI* window_ui)
	{
		m_render_pipeline->initializeUIRenderBackend(window_ui);
	}

	void RenderSystem::processSwapData()
	{
	}
}

