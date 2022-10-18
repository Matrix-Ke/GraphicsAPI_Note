#pragma once

#include "Render/RenderPass.h"

namespace Matrix
{
	class EditorUI;

	struct UIPassInitInfo : RenderPassInitInfo
	{
		VkRenderPass render_pass;
	};

	class UIPass : public RenderPass
	{
	public:
		void initialize(const RenderPassInitInfo* init_info) override final;
		void initializeUIRenderBackend(EditorUI* window_ui) override final;
		void draw() override final;

	private:
		void uploadFonts();

	private:
		EditorUI* m_window_ui;
	};
}
