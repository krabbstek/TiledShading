#include "ClusteredDeferredComputeLightTilesPass.h"

#include "Globals.h"

#include <algorithm>

ClusteredDeferredComputeLightTilesPass::ClusteredDeferredComputeLightTilesPass(
	Renderer& renderer,
	std::shared_ptr<GLShader> computeShader,
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture,
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO)
	: RenderPass(renderer, computeShader),
	m_ViewSpacePositionTexture(viewSpacePositionTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(&g_Lights, sizeof(g_Lights))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO)
{
	TileGrid* tileGrid = new TileGrid();

	m_LeftPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(tileGrid->leftPlanes, sizeof(tileGrid->leftPlanes));
	m_RightPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(tileGrid->rightPlanes, sizeof(tileGrid->rightPlanes));
	m_BottomPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(tileGrid->bottomPlanes, sizeof(tileGrid->bottomPlanes));
	m_TopPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(tileGrid->topPlanes, sizeof(tileGrid->topPlanes));

	delete tileGrid;

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
	m_Shader->SetUniform1f("u_NearPlaneDepth", g_NearPlaneDepth);
	m_Shader->SetUniform1f("u_FarPlaneDepth", g_FarPlaneDepth);
	m_Shader->SetUniformMat4("u_ProjectionMatrix", m_Renderer.camera.projectionMatrix);
}

ClusteredDeferredComputeLightTilesPass::~ClusteredDeferredComputeLightTilesPass()
{
}


void ClusteredDeferredComputeLightTilesPass::Render(std::vector<Renderable*>& renderables)
{
	m_ViewSpacePositionTexture->Bind(0);
	
	int zero = 0;

	m_LightSSBO->SetData(&g_Lights, sizeof(g_Lights));
	m_LightIndexSSBO->SetSubData(&zero, 0, sizeof(zero));

	m_LightSSBO->Bind(3);
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->Bind(5);

	m_LeftPlanesSSBO->Bind(10);
	m_RightPlanesSSBO->Bind(11);
	m_BottomPlanesSSBO->Bind(12);
	m_TopPlanesSSBO->Bind(13);

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
	m_Shader->DispatchComputeShader(g_NumTileCols, g_NumTileRows, 1);
}