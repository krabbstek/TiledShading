#pragma once

#include "meshes/FullscreenMesh.h"
#include "graphics/RenderPass.h"
#include "graphics/opengl/OpenGL.h"

class TiledDeferredClearTileMinMaxDepthPass : public RenderPass
{
public:
	TiledDeferredClearTileMinMaxDepthPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture, std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture);
	~TiledDeferredClearTileMinMaxDepthPass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

protected:
	std::shared_ptr<GLImageTexture2D> m_TileMinDepthImageTexture;
	std::shared_ptr<GLImageTexture2D> m_TileMaxDepthImageTexture;
	FullscreenMesh m_FullscreenMesh;
};