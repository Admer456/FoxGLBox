#include "IRenderWorld.hpp"
#include "Model.hpp"
#include "IRenderer.hpp"
#include "BackendRegistry.hpp"

extern IRenderer* AllocateRenderer45();

IRenderer* GetRendererByBackend( int backend )
{
	IRenderer* renderer = BackendRegistry::AllocateByBackend( backend );

	if ( nullptr == renderer && backend == RenderInitParams::Renderer_OpenGL45 )
	{
		renderer = AllocateRenderer45();
	}

	return renderer;
}

BackendRegistry* BackendRegistry::head = nullptr;
