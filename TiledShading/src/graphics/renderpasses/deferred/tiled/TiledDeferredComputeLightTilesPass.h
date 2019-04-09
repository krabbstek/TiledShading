#pragma once

#include "graphics/RenderPass.h"
#include "graphics/TileGrid.h"

class TiledDeferredComputeLightTilesPass : public RenderPass
{
public:
	TiledDeferredComputeLightTilesPass(
		Renderer& renderer,
		std::shared_ptr<GLShader> computeShader,
		std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture,
		std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture,
		std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
		std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO);
	~TiledDeferredComputeLightTilesPass();

	virtual void Render(std::vector<Renderable*>& renderables) override;

protected:
	TileGrid m_TileGrid;
	std::shared_ptr<GLImageTexture2D> m_TileMinDepthImageTexture;
	std::shared_ptr<GLImageTexture2D> m_TileMaxDepthImageTexture;
	std::shared_ptr<GLShaderStorageBuffer> m_LightSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_LightIndexSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_TileIndexSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_LeftPlanesSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_RightPlanesSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_BottomPlanesSSBO;
	std::shared_ptr<GLShaderStorageBuffer> m_TopPlanesSSBO;
};