#pragma once
#include "Render/RenderType.h"
#include "Render/RenderCommon.h"


#include <memory>
#include <string>
#include <unordered_map>

namespace Matrix
{
	class VulkanRHI;
	class RenderScene;
	class RenderCamera;
	class AxisAlignedBox;


	struct StorageBuffer
	{
		// limits
		uint32_t _min_uniform_buffer_offset_alignment{ 256 };
		uint32_t _min_storage_buffer_offset_alignment{ 256 };
		uint32_t _max_storage_buffer_range{ 1 << 27 };
		uint32_t _non_coherent_atom_size{ 256 };

		VkBuffer              _global_upload_ringbuffer;
		VkDeviceMemory        _global_upload_ringbuffer_memory;
		void* _global_upload_ringbuffer_memory_pointer;
		std::vector<uint32_t> _global_upload_ringbuffers_begin;
		std::vector<uint32_t> _global_upload_ringbuffers_end;
		std::vector<uint32_t> _global_upload_ringbuffers_size;

		VkBuffer       _global_null_descriptor_storage_buffer;
		VkDeviceMemory _global_null_descriptor_storage_buffer_memory;

		// axis
		VkBuffer       _axis_inefficient_storage_buffer;
		VkDeviceMemory _axis_inefficient_storage_buffer_memory;
		void* _axis_inefficient_storage_buffer_memory_pointer;
	};

	struct GlobalRenderResource
	{
		StorageBuffer        _storage_buffer;
	};


	class RenderResourceBase
	{
	public:
		virtual ~RenderResourceBase() {}

		virtual void uploadGlobalRenderResource(std::shared_ptr<VulkanRHI> rhi, LevelResourceDesc level_resource_desc) = 0;

		virtual void updatePerFrameBuffer(std::shared_ptr<RenderScene>  render_scene,
			std::shared_ptr<RenderCamera> camera) = 0;

		// TODO: data caching
		std::shared_ptr<TextureData> loadTextureHDR(std::string file, int desired_channels = 4);
		std::shared_ptr<TextureData> loadTexture(std::string file, bool is_srgb = false);
		RenderMeshData               loadMeshData(const MeshSourceDesc& source, AxisAlignedBox& bounding_box);
		RenderMaterialData           loadMaterialData(const MaterialSourceDesc& source);
		AxisAlignedBox               getCachedBoudingBox(const MeshSourceDesc& source) const;

	private:
		StaticMeshData loadStaticMesh(std::string mesh_file, AxisAlignedBox& bounding_box);

		std::unordered_map<MeshSourceDesc, AxisAlignedBox> m_bounding_box_cache_map;
	};

	class RenderResource :public RenderResourceBase
	{
	public:
		virtual void updatePerFrameBuffer(std::shared_ptr<RenderScene>  render_scene, std::shared_ptr<RenderCamera> camera) = 0;
		void resetRingBufferOffset(uint8_t current_frame_index);

	public:
		std::shared_ptr<TextureData> loadTexture(std::string file, bool is_srgb = false);

		RenderMeshData               loadMeshData(const MeshSourceDesc& source, AxisAlignedBox& bounding_box);

		StaticMeshData				loadStaticMesh(std::string mesh_file, AxisAlignedBox& bounding_box);

		std::unordered_map<MeshSourceDesc, AxisAlignedBox> m_bounding_box_cache_map;

	public:
		void updateMeshData(std::shared_ptr<VulkanRHI>                          rhi,
			bool                                          enable_vertex_blending,
			uint32_t                                      index_buffer_size,
			void* index_buffer_data,
			uint32_t                                      vertex_buffer_size,
			struct MeshVertexDataDefinition const* vertex_buffer_data,
			uint32_t                                      joint_binding_buffer_size,
			struct MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
			VulkanMesh& now_mesh);
		void updateVertexBuffer(std::shared_ptr<VulkanRHI>                          rhi,
			bool                                          enable_vertex_blending,
			uint32_t                                      vertex_buffer_size,
			struct MeshVertexDataDefinition const* vertex_buffer_data,
			uint32_t                                      joint_binding_buffer_size,
			struct MeshVertexBindingDataDefinition const* joint_binding_buffer_data,
			uint32_t                                      index_buffer_size,
			uint16_t* index_buffer_data,
			VulkanMesh& now_mesh);
		void updateIndexBuffer(std::shared_ptr<VulkanRHI> rhi,
			uint32_t             index_buffer_size,
			void* index_buffer_data,
			VulkanMesh& now_mesh);
		void updateTextureImageData(std::shared_ptr<VulkanRHI> rhi, const TextureDataToUpdate& texture_data);

	public:
		// global rendering resource, include global storage buffer
		GlobalRenderResource m_global_render_resource;

		//// storage buffer objects
		MeshPerframeStorageBufferObject                 m_mesh_perframe_storage_buffer_object;
		MeshPointLightShadowPerframeStorageBufferObject	m_mesh_point_light_shadow_perframe_storage_buffer_object;
		//MeshDirectionalLightShadowPerframeStorageBufferObject
		//	m_mesh_directional_light_shadow_perframe_storage_buffer_object;
		//AxisStorageBufferObject                        m_axis_storage_buffer_object;
		//MeshInefficientPickPerframeStorageBufferObject m_mesh_inefficient_pick_perframe_storage_buffer_object;
		//ParticleBillboardPerframeStorageBufferObject   m_particlebillboard_perframe_storage_buffer_object;
		//ParticleCollisionPerframeStorageBufferObject   m_particle_collision_perframe_storage_buffer_object;

		//// cached mesh and material
		//std::map<size_t, VulkanMesh>        m_vulkan_meshes;
		//std::map<size_t, VulkanPBRMaterial> m_vulkan_pbr_materials;

		// descriptor set layout in main camera pass will be used when uploading resource
		const VkDescriptorSetLayout* m_mesh_descriptor_set_layout{ nullptr };
		const VkDescriptorSetLayout* m_material_descriptor_set_layout{ nullptr };
	};

}