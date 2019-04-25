#include "Globals.h"
#include "Plane.h"
#include "graphics/Light.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTechnique.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/renderpasses/StartTimerPass.h"
#include "graphics/renderpasses/StopTimerPass.h"
#include "graphics/renderpasses/PlotTimersPass.h"
#include "graphics/renderpasses/ClearDefaultFramebufferPass.h"
#include "graphics/renderpasses/forward/ForwardPass.h"
#include "graphics/renderpasses/forward/ForwardPrepass.h"
#include "graphics/renderpasses/forward/tiled/TiledForwardPrepass.h"
#include "graphics/renderpasses/forward/tiled/TiledForwardComputeLightTilesPass.h"
#include "graphics/renderpasses/forward/tiled/TiledForwardLightingPass.h"
#include "graphics/renderpasses/deferred/DeferredPrepass.h"
#include "graphics/renderpasses/deferred/DeferredLightingPass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredPrepass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredComputeLightTilesPass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredLightingPass.h"
#include "graphics/renderpasses/deferred/clustered/ClusteredDeferredPrepass.h"
#include "graphics/renderpasses/deferred/clustered/ClusteredDeferredComputeLightTilesPass.h"
#include "graphics/renderpasses/deferred/clustered/ClusteredDeferredLightingPass.h"
#include "math/math.h"
#include "meshes/Cube.h"
#include "meshes/PlaneMesh.h"

#include <chrono>
#include <iostream>
#include <random>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <stb_image.h>

#define USE_IMGUI

enum RENDER_MODE
{
	NONE = 0,
	FORWARD,
	TILED_FORWARD,
	DEFERRED,
	TILED_DEFERRED,
	CLUSTERED_DEFERRED,
	NUM_RENDER_MODES,
};

std::pair<std::shared_ptr<RenderTechnique>, std::string> renderModes[NUM_RENDER_MODES];

RENDER_MODE currentRenderMode = TILED_FORWARD;

GLFWwindow* window;

Renderer renderer;

Material material;

int Init();
void InitNoRendering();
void InitForwardRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void InitTiledForwardRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void InitDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void InitTiledDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void InitClusteredDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void ImGuiRender();

void PrintBinary(unsigned int x)
{
	std::printf("0b");
	for (int i = 0; i < 32; i++)
	{
		std::printf("%d", (x >> i) & 1);
	}
}

int main()
{
	static_assert(!((g_WindowWidth % g_TileSize) | (g_WindowHeight % g_TileSize)));

	std::chrono::time_point<std::chrono::high_resolution_clock> prevTime, currentTime;
	prevTime = std::chrono::high_resolution_clock::now();

	int initResult = Init();
	if (initResult)
		return initResult;

	{
		renderer.camera.projectionMatrix = mat4::Perspective(g_FOV, g_AspectRatio, g_NearPlaneDepth, g_FarPlaneDepth);
		renderer.camera.position = vec3(0.0f, 4.0f, 12.0f);
		renderer.camera.rotation = vec3(-0.4f, 0.0f, 0.0f);
		mat4 V = renderer.camera.GetViewMatrix();

		Cube cubeGrid[g_CubeGridSize][g_CubeGridSize];
		for (int x = 0; x < g_CubeGridSize; x++)
			for (int z = 0; z < g_CubeGridSize; z++)
				//cubeGrid[x][z].position = vec3(2.0f * (x - 4.0f), 0.0f, 2.0f * (z - 4.0f));
				cubeGrid[x][z].position = vec3(16.0f / (g_CubeGridSize - 1) * (x - 0.5f * (g_CubeGridSize - 1)), 0.0f, 16.0f / (g_CubeGridSize - 1) * (z - 0.5f * (g_CubeGridSize - 1)));
		PlaneMesh planeMesh({ 0.0f, -0.5f, 0.0f }, { 24.0f, 24.0f });

		g_Lights.globalLight.color = vec4(g_GlobalLightIntensity, g_GlobalLightIntensity, g_GlobalLightIntensity, 1.0f);
		g_Lights.globalLight.viewSpacePosition = V * g_GlobalLightPosition;
		for (int x = 0; x < g_LightGridSize; x++)
		{
			for (int z = 0; z < g_LightGridSize; z++)
			{
				g_LightColors[x][z] = vec3(float(rand()) * (1.0f / float(RAND_MAX)), float(rand()) * (1.0f / float(RAND_MAX)), float(rand()) * (1.0f / float(RAND_MAX)));
				g_Lights.lightGrid[x][z].color = vec4(g_LightIntensityMultiplier * g_LightColors[x][z].r, g_LightIntensityMultiplier * g_LightColors[x][z].g, g_LightIntensityMultiplier * g_LightColors[x][z].b, 1.0f);

				g_Lights.lightGrid[x][z].viewSpacePosition = V * vec4(g_LightGridScale.x * (x - (g_LightGridSize - 1) / 2) + g_LightGridOffset.x, g_LightGridOffset.y, g_LightGridScale.y * (z - (g_LightGridSize - 1) / 2) + g_LightGridOffset.z, 1.0f);;
			}
		}

		material.albedo = vec4(0.2f, 0.3f, 0.8f);
		material.reflectivity = 1.0;
		material.shininess = 1.0;
		material.metalness = 0.5;
		material.fresnel = 0.5;

		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glCullFace(GL_BACK));
		GLCall(glDepthFunc(GL_LEQUAL));

		unsigned int depthMask = ~0;
		float depth = 0.5f;
		float radius = 0.2f;
		unsigned int index1 = unsigned int(32.0f * (depth - radius));
		unsigned int index2 = 31 - unsigned int(32.0f * (depth + radius));
		depthMask = (depthMask << index1) >> index1;
		depthMask = (depthMask >> index2) << index2;
		PrintBinary(depthMask);
		std::printf("\n");

		/// Render techniques
		{
			// Timers
			std::shared_ptr<GLTimer> totalRenderTimer = std::make_shared<GLTimer>();
			std::shared_ptr<GLTimer> prepassTimer = std::make_shared<GLTimer>();
			std::shared_ptr<GLTimer> lightingPassTimer = std::make_shared<GLTimer>();

			std::shared_ptr<GLTimer> tileLightingComputationTimer = std::make_shared<GLTimer>();

			// No rendering
			InitNoRendering();

			// Forward non-tiled rendering
			InitForwardRendering(prepassTimer, tileLightingComputationTimer, lightingPassTimer, totalRenderTimer);

			// Tiled forward rendering
			InitTiledForwardRendering(prepassTimer, tileLightingComputationTimer, lightingPassTimer, totalRenderTimer);

			// Deferred non-tiled rendering
			InitDeferredRendering(prepassTimer, tileLightingComputationTimer, lightingPassTimer, totalRenderTimer);

			// Tiled deferred rendering
			InitTiledDeferredRendering(prepassTimer, tileLightingComputationTimer, lightingPassTimer, totalRenderTimer);

			// Clustered deferred rendering
			InitClusteredDeferredRendering(prepassTimer, tileLightingComputationTimer, lightingPassTimer, totalRenderTimer);
		}
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

#ifdef USE_IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif

			currentTime = std::chrono::high_resolution_clock::now();
			float dt = float((currentTime - prevTime).count()) * 1e-9f;
			prevTime = currentTime;

			g_Lights.globalLight.color = vec4(g_GlobalLightIntensity, g_GlobalLightIntensity, g_GlobalLightIntensity, g_LightFalloffThreshold / g_GlobalLightIntensity);
			g_Lights.globalLight.viewSpacePosition = V * g_GlobalLightPosition;
			if (g_DynamicLights)
			{
				g_Time += dt;

				for (int x = 0; x < g_LightGridSize; x++)
				{
					for (int z = 0; z < g_LightGridSize; z++)
					{
						g_Lights.lightGrid[x][z].color = vec4(g_LightIntensityMultiplier * g_LightColors[x][z].r, g_LightIntensityMultiplier * g_LightColors[x][z].g, g_LightIntensityMultiplier * g_LightColors[x][z].b, g_LightFalloffThreshold / g_LightIntensityMultiplier);

						g_Lights.lightGrid[x][z].viewSpacePosition = V * mat4::RotateY(0.3f * g_Time) * vec4(g_LightGridScale.x * (x - (g_LightGridSize - 1) / 2) + g_LightGridOffset.x, g_LightGridOffset.y, g_LightGridScale.y * (z - (g_LightGridSize - 1) / 2) + g_LightGridOffset.z, 1.0f);
					}
				}
			}
			else
			{
				for (int x = 0; x < g_LightGridSize; x++)
					for (int z = 0; z < g_LightGridSize; z++)
						g_Lights.lightGrid[x][z].color = vec4(g_LightIntensityMultiplier * g_LightColors[x][z].r, g_LightIntensityMultiplier * g_LightColors[x][z].g, g_LightIntensityMultiplier * g_LightColors[x][z].b, g_LightFalloffThreshold / g_LightIntensityMultiplier);
			}

			// Render
			std::shared_ptr<RenderTechnique> currentRenderTechnique = renderModes[currentRenderMode].first;
			currentRenderTechnique->Render(planeMesh);
			for (int x = 0; x < g_CubeGridSize; x++)
				for (int y = 0; y < g_CubeGridSize; y++)
					currentRenderTechnique->Render(cubeGrid[x][y]);
			currentRenderTechnique->Render();
			
#ifdef USE_IMGUI
			ImGuiRender();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
			
			glfwSwapBuffers(window);
		}
	}

	for (int i = 0; i < NUM_RENDER_MODES; i++)
		renderModes[i].first.reset();

	ImGui_ImplGlfw_Shutdown();

	glfwTerminate();

	return 0;
}


int Init()
{
	int glfwResult = glfwInit();
	if (!glfwResult)
	{
		std::printf("Failed to initialize GLFW!\n");
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(g_WindowWidth, g_WindowHeight, "TiledShading", NULL, NULL);
	if (!window)
	{
		std::printf("Failed to create glfwWindow!\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwDestroyWindow(window);
		glfwTerminate();
		return -1;
	}

#ifdef USE_IMGUI
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 150");
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#endif

	stbi_set_flip_vertically_on_load(1);

	return 0;
}


void ImGuiRender()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	ImGui::Begin("Config");

	ImGui::Text("Render mode");
	for (int i = 0; i < NUM_RENDER_MODES; i++)
		ImGui::RadioButton(renderModes[i].second.c_str(), (int*)&currentRenderMode, i);
	ImGui::SliderFloat("Heatmap alpha", &g_HeatmapAlpha, 0.0, 1.0, "%.1f", 1.0f);

	ImGui::Separator();

	ImGui::Text("Global light");
	ImGui::SliderFloat("Global light intensity multiplier", &g_GlobalLightIntensity, 0.0f, 10000.0f, "%.2f", 3.0f);
	ImGui::SliderFloat3("Global light position ", &g_GlobalLightPosition.x, -25.0f, 25.0f, "%.1f", 1.0f);

	ImGui::Separator();

	ImGui::Text("Light grid");
	ImGui::Checkbox("Dynamic lights", &g_DynamicLights);
	ImGui::SliderFloat("Light intensity multiplier", &g_LightIntensityMultiplier, 0.0f, 100.0f, "%.2f", 3.0f);
	ImGui::SliderFloat("Light falloff threshold", &g_LightFalloffThreshold, 0.001f, 0.1f, "%.3f", 2.0f);
	ImGui::SliderFloat3("Light grid offset", &g_LightGridOffset.x, -3.0f, 3.0f, "%.1f", 1.0f);
	ImGui::SliderFloat2("Light grid scale", &g_LightGridScale.x, 0.0f, 10.0f, "%.1f", 1.0f);

	ImGui::Separator();

	ImGui::Text("Material");
	ImGui::ColorEdit3("Albedo", &material.albedo.r);
	ImGui::SliderFloat("Reflectivity", &material.reflectivity, 0.0f, 1.0f, "%.2f", 1.0f);
	ImGui::SliderFloat("Shininess", &material.shininess, 0.0f, 1000.0f, "%.2f", 3.0f);
	ImGui::SliderFloat("Metalness", &material.metalness, 0.0f, 1.0f, "%.2f", 1.0f);
	ImGui::SliderFloat("Fresnel", &material.fresnel, 0.0f, 1.0f, "%.2f", 1.0f);

	ImGui::End();
}


void InitNoRendering()
{
	std::shared_ptr<ClearDefaultFramebufferPass> clearDefaultFramebufferPass = std::make_shared<ClearDefaultFramebufferPass>(renderer, std::shared_ptr<GLShader>());

	std::shared_ptr<RenderTechnique> noRendering = std::make_shared<RenderTechnique>();
	noRendering->AddRenderPass(clearDefaultFramebufferPass);

	renderModes[NONE].first = noRendering;
	renderModes[NONE].second = "No rendering";
}

void InitForwardRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
{
	// Shaders
	std::shared_ptr<GLShader> forwardShader = std::make_shared<GLShader>();
	forwardShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/forward/forward_vs.glsl");
	forwardShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/forward/forward_fs.glsl");
	forwardShader->CompileShaders();

	// Render passes
	std::shared_ptr<ForwardPass> forwardPass = std::make_shared<ForwardPass>(
		renderer,
		forwardShader,
		material
	);

	// Timers
	std::shared_ptr<StartTimerPass> startPrepassRenderTimePass = std::make_shared<StartTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopTimerPass> stopPrepassRenderTimePass = std::make_shared<StopTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartTimerPass> startTileLightingComputationTimePass = std::make_shared<StartTimerPass>(renderer, tileLightingComputationTimer);
	std::shared_ptr<StopTimerPass> stopTileLightingComputationTimePass = std::make_shared<StopTimerPass>(renderer, tileLightingComputationTimer);

	std::shared_ptr<StartTimerPass> startLightingPassRenderTimePass = std::make_shared<StartTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartTimerPass> startTotalRenderTimePass = std::make_shared<StartTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopTimerPass> stopTotalRenderTimePass = std::make_shared<StopTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddLargeTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddSmallTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddSmallTimer("Tile lighting computation time", tileLightingComputationTimer);
	plotTimersPass->AddSmallTimer("Lighting pass render time", lightingPassTimer);

	// Render technique
	std::shared_ptr<RenderTechnique> forwardRendering = std::make_shared<RenderTechnique>();
	// Plot timers
	forwardRendering->AddRenderPass(plotTimersPass);
	forwardRendering->AddRenderPass(startTotalRenderTimePass);
	// Empty prepass
	forwardRendering->AddRenderPass(startPrepassRenderTimePass);
	forwardRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Empty compute shader pass
	forwardRendering->AddRenderPass(startTileLightingComputationTimePass);
	forwardRendering->AddRenderPass(stopTileLightingComputationTimePass);
	// Lighting pass
	forwardRendering->AddRenderPass(startLightingPassRenderTimePass);
	forwardRendering->AddRenderPass(forwardPass);
	forwardRendering->AddRenderPass(stopLightingPassRenderTimePass);
	forwardRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[FORWARD].first = forwardRendering;
	renderModes[FORWARD].second = "Forward rendering";
}

void InitTiledForwardRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> tileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
{
	// Textures
	std::shared_ptr<GLTexture2D> viewSpaceDepthTexture = std::make_shared<GLTexture2D>();
	viewSpaceDepthTexture->Load(GL_R32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RED, GL_UNSIGNED_BYTE);
	viewSpaceDepthTexture->SetMinMagFilter(GL_NEAREST);
	viewSpaceDepthTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	// SSBOs
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, BIT(24));
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, g_NumTileRows * g_NumTileCols * 2 * sizeof(int));

	// Shaders
	std::shared_ptr<GLShader> tiledForwardPrepassShader = std::make_shared<GLShader>();
	tiledForwardPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/forward/tiled/tiled_forward_prepass_vs.glsl");
	tiledForwardPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/forward/tiled/tiled_forward_prepass_fs.glsl");
	tiledForwardPrepassShader->CompileShaders();

	std::shared_ptr<GLShader> tiledForwardComputeLightTilesShader = std::make_shared<GLShader>();
	tiledForwardComputeLightTilesShader->AddShaderFromFile(GL_COMPUTE_SHADER, "res/shaders/forward/tiled/tiled_forward_compute_light_tiles_cs.glsl");
	tiledForwardComputeLightTilesShader->CompileShaders();

	std::shared_ptr<GLShader> tiledForwardLightingPassShader = std::make_shared<GLShader>();
	tiledForwardLightingPassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/forward/tiled/tiled_forward_lighting_pass_vs.glsl");
	tiledForwardLightingPassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/forward/tiled/tiled_forward_lighting_pass_fs.glsl");
	tiledForwardLightingPassShader->CompileShaders();
	tiledForwardLightingPassShader->SetUniform1i("u_NumTileCols", g_NumTileCols);
	tiledForwardLightingPassShader->SetUniform1i("u_TileSize", g_TileSize);

	// Render passes
	std::shared_ptr<TiledForwardPrepass> tiledForwardPrepass = std::make_shared<TiledForwardPrepass>(
		renderer,
		tiledForwardPrepassShader,
		viewSpaceDepthTexture
	);

	std::shared_ptr<TiledForwardComputeLightTilesPass> tiledForwardComputeLightTilesPass = std::make_shared<TiledForwardComputeLightTilesPass>(
		renderer,
		tiledForwardComputeLightTilesShader,
		viewSpaceDepthTexture,
		lightIndexSSBO,
		tileIndexSSBO
	);

	std::shared_ptr<TiledForwardLightingPass> tiledForwardLightingPass = std::make_shared<TiledForwardLightingPass>(
		renderer,
		tiledForwardLightingPassShader,
		viewSpaceDepthTexture,
		lightIndexSSBO,
		tileIndexSSBO,
		tiledForwardPrepass->GetPrepassFramebuffer(),
		material
	);

	// Timers
	std::shared_ptr<StartTimerPass> startPrepassRenderTimePass = std::make_shared<StartTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopTimerPass> stopPrepassRenderTimePass = std::make_shared<StopTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartTimerPass> startTileLightingComputationTimePass = std::make_shared<StartTimerPass>(renderer, tileLightingComputationTimer);
	std::shared_ptr<StopTimerPass> stopTileLightingComputationTimePass = std::make_shared<StopTimerPass>(renderer, tileLightingComputationTimer);

	std::shared_ptr<StartTimerPass> startLightingPassRenderTimePass = std::make_shared<StartTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartTimerPass> startTotalRenderTimePass = std::make_shared<StartTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopTimerPass> stopTotalRenderTimePass = std::make_shared<StopTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddLargeTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddSmallTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddSmallTimer("Tile lighting computation time", tileLightingComputationTimer);
	plotTimersPass->AddSmallTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> tiledForwardRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	tiledForwardRendering->AddRenderPass(plotTimersPass);
	tiledForwardRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	tiledForwardRendering->AddRenderPass(startPrepassRenderTimePass);
	tiledForwardRendering->AddRenderPass(tiledForwardPrepass);
	tiledForwardRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Compute light tiles
	tiledForwardRendering->AddRenderPass(startTileLightingComputationTimePass);
	tiledForwardRendering->AddRenderPass(tiledForwardComputeLightTilesPass);
	tiledForwardRendering->AddRenderPass(stopTileLightingComputationTimePass);
	// Lighting
	tiledForwardRendering->AddRenderPass(startLightingPassRenderTimePass);
	tiledForwardRendering->AddRenderPass(tiledForwardLightingPass);
	tiledForwardRendering->AddRenderPass(stopLightingPassRenderTimePass);
	tiledForwardRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[TILED_FORWARD].first = tiledForwardRendering;
	renderModes[TILED_FORWARD].second = "Tiled forward rendering";
}

void InitDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> deferredTileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
{
	// Textures
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture = std::make_shared<GLTexture2D>();
	viewSpacePositionTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpacePositionTexture->SetMinMagFilter(GL_NEAREST);
	viewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLTexture2D> viewSpaceNormalTexture = std::make_shared<GLTexture2D>();
	viewSpaceNormalTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpaceNormalTexture->SetMinMagFilter(GL_NEAREST);
	viewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	// Shaders
	std::shared_ptr<GLShader> deferredPrepassShader = std::make_shared<GLShader>();
	deferredPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/deferred_prepass_vs.glsl");
	deferredPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/deferred_prepass_fs.glsl");
	deferredPrepassShader->CompileShaders();

	std::shared_ptr<GLShader> deferredLightingPassShader = std::make_shared<GLShader>();
	deferredLightingPassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/deferred_lighting_pass_vs.glsl");
	deferredLightingPassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/deferred_lighting_pass_fs.glsl");
	deferredLightingPassShader->CompileShaders();

	// Render passes
	std::shared_ptr<DeferredPrepass> deferredPrepass = std::make_shared<DeferredPrepass>(
		renderer,
		deferredPrepassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture
	);

	std::shared_ptr<DeferredLightingPass> deferredLightingPass = std::make_shared<DeferredLightingPass>(
		renderer,
		deferredLightingPassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture,
		material
	);

	// Timers
	std::shared_ptr<StartTimerPass> startPrepassRenderTimePass = std::make_shared<StartTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopTimerPass> stopPrepassRenderTimePass = std::make_shared<StopTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartTimerPass> startTileLightingComputationTimePass = std::make_shared<StartTimerPass>(renderer, deferredTileLightingComputationTimer);
	std::shared_ptr<StopTimerPass> stopTileLightingComputationTimePass = std::make_shared<StopTimerPass>(renderer, deferredTileLightingComputationTimer);

	std::shared_ptr<StartTimerPass> startLightingPassRenderTimePass = std::make_shared<StartTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartTimerPass> startTotalRenderTimePass = std::make_shared<StartTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopTimerPass> stopTotalRenderTimePass = std::make_shared<StopTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddLargeTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddSmallTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddSmallTimer("Tile lighting computation time", deferredTileLightingComputationTimer);
	plotTimersPass->AddSmallTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> deferredRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	deferredRendering->AddRenderPass(plotTimersPass);
	deferredRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	deferredRendering->AddRenderPass(startPrepassRenderTimePass);
	deferredRendering->AddRenderPass(deferredPrepass);
	deferredRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Tile lighting computation
	deferredRendering->AddRenderPass(startTileLightingComputationTimePass);
	deferredRendering->AddRenderPass(stopTileLightingComputationTimePass);
	// Lighting
	deferredRendering->AddRenderPass(startLightingPassRenderTimePass);
	deferredRendering->AddRenderPass(deferredLightingPass);
	deferredRendering->AddRenderPass(stopLightingPassRenderTimePass);
	deferredRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[DEFERRED].first = deferredRendering;
	renderModes[DEFERRED].second = "Deferred rendering";
}

void InitTiledDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> deferredTileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
{
	// Textures
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture = std::make_shared<GLTexture2D>();
	viewSpacePositionTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpacePositionTexture->SetMinMagFilter(GL_NEAREST);
	viewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLTexture2D> viewSpaceNormalTexture = std::make_shared<GLTexture2D>();
	viewSpaceNormalTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpaceNormalTexture->SetMinMagFilter(GL_NEAREST);
	viewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	// SSBOs
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, BIT(24));
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, g_NumTileRows * g_NumTileCols * 2 * sizeof(int));

	// Shaders
	std::shared_ptr<GLShader> tiledDeferredPrepassShader = std::make_shared<GLShader>();
	tiledDeferredPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/tiled/tiled_deferred_prepass_vs.glsl");
	tiledDeferredPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/tiled/tiled_deferred_prepass_fs.glsl");
	tiledDeferredPrepassShader->CompileShaders();

	std::shared_ptr<GLShader> tiledDeferredLightingPassShader = std::make_shared<GLShader>();
	tiledDeferredLightingPassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/tiled/tiled_deferred_lighting_pass_vs.glsl");
	tiledDeferredLightingPassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/tiled/tiled_deferred_lighting_pass_fs.glsl");
	tiledDeferredLightingPassShader->CompileShaders();
	tiledDeferredLightingPassShader->SetUniform1i("u_NumTileCols", g_NumTileCols);
	tiledDeferredLightingPassShader->SetUniform1i("u_TileSize", g_TileSize);

	std::shared_ptr<GLShader> tiledDeferredComputeLightTilesShader = std::make_shared<GLShader>();
	tiledDeferredComputeLightTilesShader->AddShaderFromFile(GL_COMPUTE_SHADER, "res/shaders/deferred/tiled/tiled_deferred_compute_light_tiles_cs.glsl");
	tiledDeferredComputeLightTilesShader->CompileShaders();

	// Render passes
	std::shared_ptr<TiledDeferredPrepass> tiledDeferredPrepass = std::make_shared<TiledDeferredPrepass>(
		renderer,
		tiledDeferredPrepassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture
	);

	std::shared_ptr<TiledDeferredComputeLightTilesPass> tiledDeferredComputeLightTilesPass = std::make_shared<TiledDeferredComputeLightTilesPass>(
		renderer,
		tiledDeferredComputeLightTilesShader,
		viewSpacePositionTexture,
		lightIndexSSBO,
		tileIndexSSBO
	);

	std::shared_ptr<TiledDeferredLightingPass> tiledDeferredLightingPass = std::make_shared<TiledDeferredLightingPass>(
		renderer,
		tiledDeferredLightingPassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture,
		lightIndexSSBO,
		tileIndexSSBO,
		material
	);

	// Timers
	std::shared_ptr<StartTimerPass> startPrepassRenderTimePass = std::make_shared<StartTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopTimerPass> stopPrepassRenderTimePass = std::make_shared<StopTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartTimerPass> startTileLightingComputationTimePass = std::make_shared<StartTimerPass>(renderer, deferredTileLightingComputationTimer);
	std::shared_ptr<StopTimerPass> stopTileLightingComputationTimePass = std::make_shared<StopTimerPass>(renderer, deferredTileLightingComputationTimer);

	std::shared_ptr<StartTimerPass> startLightingPassRenderTimePass = std::make_shared<StartTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartTimerPass> startTotalRenderTimePass = std::make_shared<StartTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopTimerPass> stopTotalRenderTimePass = std::make_shared<StopTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddLargeTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddSmallTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddSmallTimer("Tile lighting computation time", deferredTileLightingComputationTimer);
	plotTimersPass->AddSmallTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> tiledDeferredRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	tiledDeferredRendering->AddRenderPass(plotTimersPass);
	tiledDeferredRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	tiledDeferredRendering->AddRenderPass(startPrepassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredPrepass);
	tiledDeferredRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Compute light tiles
	tiledDeferredRendering->AddRenderPass(startTileLightingComputationTimePass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredComputeLightTilesPass);
	tiledDeferredRendering->AddRenderPass(stopTileLightingComputationTimePass);
	// Lighting
	tiledDeferredRendering->AddRenderPass(startLightingPassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredLightingPass);
	tiledDeferredRendering->AddRenderPass(stopLightingPassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[TILED_DEFERRED].first = tiledDeferredRendering;
	renderModes[TILED_DEFERRED].second = "Tiled deferred rendering";
}

void InitClusteredDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<Timer> deferredTileLightingComputationTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
{
	// Textures
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture = std::make_shared<GLTexture2D>();
	viewSpacePositionTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpacePositionTexture->SetMinMagFilter(GL_NEAREST);
	viewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLTexture2D> viewSpaceNormalTexture = std::make_shared<GLTexture2D>();
	viewSpaceNormalTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpaceNormalTexture->SetMinMagFilter(GL_NEAREST);
	viewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	// SSBOs
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, BIT(24));
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, g_NumTileRows * g_NumTileCols * 2 * sizeof(int));

	// Shaders
	std::shared_ptr<GLShader> clusteredDeferredPrepassShader = std::make_shared<GLShader>();
	clusteredDeferredPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/clustered/clustered_deferred_prepass_vs.glsl");
	clusteredDeferredPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/clustered/clustered_deferred_prepass_fs.glsl");
	clusteredDeferredPrepassShader->CompileShaders();

	std::shared_ptr<GLShader> clusteredDeferredLightingPassShader = std::make_shared<GLShader>();
	clusteredDeferredLightingPassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/clustered/clustered_deferred_lighting_pass_vs.glsl");
	clusteredDeferredLightingPassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/clustered/clustered_deferred_lighting_pass_fs.glsl");
	clusteredDeferredLightingPassShader->CompileShaders();
	clusteredDeferredLightingPassShader->SetUniform1i("u_NumTileCols", g_NumTileCols);
	clusteredDeferredLightingPassShader->SetUniform1i("u_TileSize", g_TileSize);

	std::shared_ptr<GLShader> clusteredDeferredComputeLightTilesShader = std::make_shared<GLShader>();
	clusteredDeferredComputeLightTilesShader->AddShaderFromFile(GL_COMPUTE_SHADER, "res/shaders/deferred/clustered/clustered_deferred_compute_light_tiles_cs.glsl");
	clusteredDeferredComputeLightTilesShader->CompileShaders();

	// Render passes
	std::shared_ptr<ClusteredDeferredPrepass> clusteredDeferredPrepass = std::make_shared<ClusteredDeferredPrepass>(
		renderer,
		clusteredDeferredPrepassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture
	);

	std::shared_ptr<ClusteredDeferredComputeLightTilesPass> clusteredDeferredComputeLightTilesPass = std::make_shared<ClusteredDeferredComputeLightTilesPass>(
		renderer,
		clusteredDeferredComputeLightTilesShader,
		viewSpacePositionTexture,
		lightIndexSSBO,
		tileIndexSSBO
	);

	std::shared_ptr<ClusteredDeferredLightingPass> clusteredDeferredLightingPass = std::make_shared<ClusteredDeferredLightingPass>(
		renderer,
		clusteredDeferredLightingPassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture,
		lightIndexSSBO,
		tileIndexSSBO,
		material
	);

	// Timers
	std::shared_ptr<StartTimerPass> startPrepassRenderTimePass = std::make_shared<StartTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopTimerPass> stopPrepassRenderTimePass = std::make_shared<StopTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartTimerPass> startTileLightingComputationTimePass = std::make_shared<StartTimerPass>(renderer, deferredTileLightingComputationTimer);
	std::shared_ptr<StopTimerPass> stopTileLightingComputationTimePass = std::make_shared<StopTimerPass>(renderer, deferredTileLightingComputationTimer);

	std::shared_ptr<StartTimerPass> startLightingPassRenderTimePass = std::make_shared<StartTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartTimerPass> startTotalRenderTimePass = std::make_shared<StartTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopTimerPass> stopTotalRenderTimePass = std::make_shared<StopTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddLargeTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddSmallTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddSmallTimer("Tile lighting computation time", deferredTileLightingComputationTimer);
	plotTimersPass->AddSmallTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> clusteredDeferredRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	clusteredDeferredRendering->AddRenderPass(plotTimersPass);
	clusteredDeferredRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	clusteredDeferredRendering->AddRenderPass(startPrepassRenderTimePass);
	clusteredDeferredRendering->AddRenderPass(clusteredDeferredPrepass);
	clusteredDeferredRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Compute light tiles
	clusteredDeferredRendering->AddRenderPass(startTileLightingComputationTimePass);
	clusteredDeferredRendering->AddRenderPass(clusteredDeferredComputeLightTilesPass);
	clusteredDeferredRendering->AddRenderPass(stopTileLightingComputationTimePass);
	// Lighting
	clusteredDeferredRendering->AddRenderPass(startLightingPassRenderTimePass);
	clusteredDeferredRendering->AddRenderPass(clusteredDeferredLightingPass);
	clusteredDeferredRendering->AddRenderPass(stopLightingPassRenderTimePass);
	clusteredDeferredRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[CLUSTERED_DEFERRED].first = clusteredDeferredRendering;
	renderModes[CLUSTERED_DEFERRED].second = "Clustered deferred rendering";
}