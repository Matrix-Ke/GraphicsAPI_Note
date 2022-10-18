#include "Engine.h"
#include "Render/RenderSystem.h"

void Matrix::MatrixEngine::startEngine(const std::string& config_file_path)
{
}

void Matrix::MatrixEngine::shutdownEngine()
{
}

void Matrix::MatrixEngine::clear()
{
}

bool Matrix::MatrixEngine::tick(float delta_time)
{

	//render
	gRuntimeGlobalContext.m_render_system->tick();

}
