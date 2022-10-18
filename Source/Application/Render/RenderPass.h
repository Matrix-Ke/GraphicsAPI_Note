#pragma once
#include "Render/RenderResource.h"


namespace Matrix
{
	class RenderResourceBase;
	class EditorUI;

	enum
	{
		_main_camera_pass_gbuffer_a = 0,
		_main_camera_pass_gbuffer_b = 1,
		_main_camera_pass_gbuffer_c = 2,
		_main_camera_pass_backup_buffer_odd = 3,
		_main_camera_pass_backup_buffer_even = 4,
		_main_camera_pass_post_process_buffer_odd = 5,
		_main_camera_pass_post_process_buffer_even = 6,
		_main_camera_pass_depth = 7,
		_main_camera_pass_swap_chain_image = 8,
		_main_camera_pass_custom_attachment_count = 5,
		_main_camera_pass_post_process_attachment_count = 2,
		_main_camera_pass_attachment_count = 9,
	};

	enum
	{
		_main_camera_subpass_basepass = 0,
		_main_camera_subpass_deferred_lighting,
		_main_camera_subpass_forward_lighting,
		_main_camera_subpass_tone_mapping,
		_main_camera_subpass_color_grading,
		_main_camera_subpass_fxaa,
		_main_camera_subpass_ui,
		_main_camera_subpass_combine_ui,
		_main_camera_subpass_count
	};

	struct VisiableNodes
	{
		std::vector<RenderMeshNode>* p_directional_light_visible_mesh_nodes{ nullptr };
		std::vector<RenderMeshNode>* p_point_lights_visible_mesh_nodes{ nullptr };
		std::vector<RenderMeshNode>* p_main_camera_visible_mesh_nodes{ nullptr };
		RenderAxisNode* p_axis_node{ nullptr };
	};
	struct RenderPassInitInfo
	{
	};

	struct RenderPassCommonInfo
	{
		std::shared_ptr<VulkanRHI>                rhi;
		std::shared_ptr<RenderResourceBase> render_resource;
	};

	class RenderPassBase
	{
	public:
		virtual void initialize(const RenderPassInitInfo* init_info) = 0;
		virtual void postInitialize() {};
		virtual void setCommonInfo(RenderPassCommonInfo common_info)
		{
			m_rhi = common_info.rhi;
			m_render_resource = common_info.render_resource;
		}
		virtual void preparePassData(std::shared_ptr<RenderResourceBase> render_resource) {};
		virtual void initializeUIRenderBackend(EditorUI* window_ui) {};

	protected:
		std::shared_ptr<VulkanRHI>                m_rhi;
		std::shared_ptr<RenderResourceBase> m_render_resource;
	};


	class RenderPass : public RenderPassBase
	{
	public:
		//RenderPass 存放所有pass可能公用的东西，如帧缓冲，描述符集。。。
		struct FrameBufferAttachment
		{
			VkDeviceMemory mem;

			VkImage        image;
			VkImageView    view;
			VkFormat       format;
		};

		struct Framebuffer
		{
			int           width;
			int           height;
			VkFramebuffer framebuffer;
			VkRenderPass  render_pass;

			std::vector<FrameBufferAttachment> attachments;
		};

		struct Descriptor
		{
			VkDescriptorSetLayout layout;
			VkDescriptorSet       descriptor_set;
		};

		struct RenderPipelineBase
		{
			VkPipelineLayout layout;
			VkPipeline       pipeline;
		};

		std::shared_ptr<VulkanRHI> m_vulkan_rhi{ nullptr };
		GlobalRenderResource* m_global_render_resource{ nullptr };

		std::vector<Descriptor>         m_descriptor_infos;
		std::vector<RenderPipelineBase> m_render_pipelines;
		Framebuffer                     m_framebuffer;

		void initialize(const RenderPassInitInfo* init_info) override {};
		void postInitialize() override {};

		virtual void draw() {};

		virtual VkRenderPass                       getRenderPass() const { return m_framebuffer.render_pass; };
		virtual std::vector<VkImageView>           getFramebufferImageViews() const;
		virtual std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;

		//可视的mesh节点
		static VisiableNodes m_visiable_nodes;

	};
}