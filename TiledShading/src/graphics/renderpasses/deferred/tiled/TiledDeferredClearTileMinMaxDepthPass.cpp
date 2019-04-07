#include "TiledDeferredClearTileMinMaxDepthPass.h"

#include "Globals.h"

#include <iostream>

TiledDeferredClearTileMinMaxDepthPass::TiledDeferredClearTileMinMaxDepthPass(Renderer& renderer, std::shared_ptr<GLShader> shader, std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture, std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture)
	: RenderPass(renderer, shader),
	m_TileMinDepthImageTexture(tileMinDepthImageTexture),
	m_TileMaxDepthImageTexture(tileMaxDepthImageTexture)
{
}

TiledDeferredClearTileMinMaxDepthPass::~TiledDeferredClearTileMinMaxDepthPass()
{
}


void TiledDeferredClearTileMinMaxDepthPass::Render(std::vector<Renderable*>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_NumTileCols, g_NumTileRows));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_TileMinDepthImageTexture->Bind(3);
	m_TileMaxDepthImageTexture->Bind(4);

	m_FullscreenMesh.Render(m_Renderer, *m_Shader);
}