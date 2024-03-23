#include "TiledDeferredComputeLightTilesPass.h"

#include "Globals.h"

TiledDeferredComputeLightTilesPass::TiledDeferredComputeLightTilesPass(
	Renderer& renderer,
	std::shared_ptr<GLShader> computeShader,
	std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture,
	std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture,
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO,
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO)
	: RenderPass(renderer, computeShader),
	m_TileMinDepthImageTexture(tileMinDepthImageTexture),
	m_TileMaxDepthImageTexture(tileMaxDepthImageTexture),
	m_LightSSBO(std::make_shared<GLShaderStorageBuffer>(g_LightGrid, sizeof(g_LightGrid))),
	m_LightIndexSSBO(lightIndexSSBO),
	m_TileIndexSSBO(tileIndexSSBO)
{
	m_LeftPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.leftPlanes, sizeof(m_TileGrid.leftPlanes));
	m_RightPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.rightPlanes, sizeof(m_TileGrid.rightPlanes));
	m_BottomPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.bottomPlanes, sizeof(m_TileGrid.bottomPlanes));
	m_TopPlanesSSBO = std::make_shared<GLShaderStorageBuffer>(m_TileGrid.topPlanes, sizeof(m_TileGrid.topPlanes));

	m_Shader->SetUniform1i("u_MaxNumLightsPerTile", g_MaxNumLightsPerTile);
}

TiledDeferredComputeLightTilesPass::~TiledDeferredComputeLightTilesPass()
{
}


void TiledDeferredComputeLightTilesPass::Render(std::vector<Renderable*>& renderables)
{
	int tilePixels[g_NumTileRows * g_NumTileCols];
	float tileMinDepth[g_NumTileRows * g_NumTileCols];
	float tileMaxDepth[g_NumTileRows * g_NumTileCols];

	m_TileMinDepthImageTexture->BindAsTexture();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, tilePixels));
	// Convert from ''int space'' back to view space depth
	for (int i = 0; i < g_NumTileRows * g_NumTileCols; i++)
		tileMinDepth[i] = float(tilePixels[i]) * (g_FarPlaneDepth / float(0x7FFFFFFF));

	m_TileMaxDepthImageTexture->BindAsTexture();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_INT, tilePixels));
	// Convert from ''int space'' back to view space depth
	for (int i = 0; i < g_NumTileRows * g_NumTileCols; i++)
		tileMaxDepth[i] = float(tilePixels[i]) * (g_FarPlaneDepth / float(0x7FFFFFFF));

	m_TileGrid.ComputeLightTiles((Light*)g_LightGrid, g_LightGridSize * g_LightGridSize, tileMinDepth, tileMaxDepth, *m_LightIndexSSBO, *m_TileIndexSSBO);

	//return;

	m_LightSSBO->SetData(g_LightGrid, sizeof(g_LightGrid));
	m_LightSSBO->Bind(3);
	
	
	memset(m_NumLightsPerTile, 0, sizeof(m_NumLightsPerTile));
	m_LightIndexSSBO->SetData(m_LightIndices, sizeof(m_LightIndices));
	m_LightIndexSSBO->Bind(4);
	m_TileIndexSSBO->SetData(m_NumLightsPerTile, sizeof(m_NumLightsPerTile));
	m_TileIndexSSBO->Bind(5);

	m_LeftPlanesSSBO->Bind(10);
	m_RightPlanesSSBO->Bind(11);
	m_BottomPlanesSSBO->Bind(12);
	m_TopPlanesSSBO->Bind(13);
	m_Shader->SetUniform1f("u_LightFalloffThreshold", g_LightFalloffThreshold / g_LightIntensityMultiplier);
	m_Shader->DispatchComputeShader(g_NumTileCols, g_NumTileRows, 1);

	GLCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));
	
	m_TileIndexSSBO->Bind();
	GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_NumLightsPerTile), m_NumLightsPerTile));

	m_LightIndexSSBO->Bind();
	GLCall(glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(m_LightIndices), m_LightIndices));

	std::vector<int> li;
	std::vector<int> ti;
	for (int i = 0; i < g_NumTileRows * g_NumTileCols; i++)
	{
		int size = m_NumLightsPerTile[i];
		int offset = g_MaxNumLightsPerTile * i;
		ti.emplace_back(li.size());
		ti.emplace_back(size);
		for (int j = 0; j < size; j++)
			li.emplace_back(m_LightIndices[offset + j]);
	}

	m_LightIndexSSBO->SetData(li.data(), li.size() * sizeof(int));
	m_TileIndexSSBO->SetData(ti.data(), ti.size() * sizeof(int));
}