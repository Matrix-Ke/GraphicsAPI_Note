#pragma once
#include "Render/RenderType.h"
#include "Editor/WindowsApplication.h"

#include "Math/Vector2.h"

#include <cstdint>
#include <deque>
#include <optional>
#include <string>

namespace Matrix
{
	class CameraPose
	{
	public:
		Vector3 m_position;
		Vector3 m_target;
		Vector3 m_up;
	};

	class CameraConfig
	{
	public:
		CameraPose m_pose;
		Vector2    m_aspect;
		float      m_z_far;
		float      m_z_near;
	};
	class SkyBoxIrradianceMap
	{
	public:
		std::string m_negative_x_map;
		std::string m_positive_x_map;
		std::string m_negative_y_map;
		std::string m_positive_y_map;
		std::string m_negative_z_map;
		std::string m_positive_z_map;
	};

	class SkyBoxSpecularMap
	{
	public:
		std::string m_negative_x_map;
		std::string m_positive_x_map;
		std::string m_negative_y_map;
		std::string m_positive_y_map;
		std::string m_negative_z_map;
		std::string m_positive_z_map;
	};

	class DirectionalLight
	{
	public:
		Vector3 m_direction;
		Vector3   m_color;
	};

	class GlobalRenderingRes
	{
	public:
		bool                m_enable_fxaa{ false };
		SkyBoxIrradianceMap m_skybox_irradiance_map;
		SkyBoxSpecularMap   m_skybox_specular_map;
		std::string         m_brdf_map;
		std::string         m_color_grading_map;

		Vector3            m_sky_color;
		Vector3            m_ambient_light;
		CameraConfig     m_camera_config;
		DirectionalLight m_directional_light;
	};


	//--------------------------------global render resource ----------------------------------
	struct LevelIBLResourceDesc
	{
		SkyBoxIrradianceMap m_skybox_irradiance_map;
		SkyBoxSpecularMap   m_skybox_specular_map;
		std::string         m_brdf_map;
	};

	struct LevelColorGradingResourceDesc
	{
		std::string m_color_grading_map;
	};

	struct LevelResourceDesc
	{
		LevelIBLResourceDesc          m_ibl_resource_desc;
		LevelColorGradingResourceDesc m_color_grading_resource_desc;
	};

	struct CameraSwapData
	{
		std::optional<float>            m_fov_x;
		std::optional<Matrix4x4>        m_view_matrix;
	};

	enum SwapDataType : uint8_t
	{
		LogicSwapDataType = 0,
		RenderSwapDataType,
		SwapDataTypeCount
	};

	struct RenderSwapData
	{
		std::optional<LevelResourceDesc>       m_level_resource_desc;
		std::optional<CameraSwapData>          m_camera_swap_data;
	};

	class RenderSwapContext
	{
	public:
		RenderSwapData& getLogicSwapData();
		RenderSwapData& getRenderSwapData();
		void            swapLogicRenderData();
		void            resetLevelRsourceSwapData();
		void            resetGameObjectResourceSwapData();
		void            resetGameObjectToDelete();
		void            resetCameraSwapData();
		void            resetPartilceBatchSwapData();
		void            resetEmitterTickSwapData();
		void            resetEmitterTransformSwapData();

	private:
		uint8_t        m_logic_swap_data_index{ LogicSwapDataType };
		uint8_t        m_render_swap_data_index{ RenderSwapDataType };
		RenderSwapData m_swap_data[SwapDataTypeCount];

		bool isReadyToSwap() const;
		void swap();
	};

}