#include "Globals.h"
#include "Plane.h"
#include "graphics/Light.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/RenderTechnique.h"
#include "graphics/opengl/OpenGL.h"
#include "graphics/renderpasses/StartGLTimerPass.h"
#include "graphics/renderpasses/StopGLTimerPass.h"
#include "graphics/renderpasses/PlotTimersPass.h"
#include "graphics/renderpasses/ClearDefaultFramebufferPass.h"
#include "graphics/renderpasses/forward/ForwardPrepass.h"
#include "graphics/renderpasses/deferred/DeferredPrepass.h"
#include "graphics/renderpasses/deferred/DeferredLightingPass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredClearTileMinMaxDepthPass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredPrepass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredComputeLightTilesPass.h"
#include "graphics/renderpasses/deferred/tiled/TiledDeferredLightingPass.h"
#include "math/math.h"
#include "meshes/Cube.h"
#include "meshes/PlaneMesh.h"

#include <chrono>
#include <iostream>

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
	DEFERRED,
	TILED_DEFERRED,
	NUM_RENDER_MODES,
};

std::pair<std::shared_ptr<RenderTechnique>, std::string> renderModes[NUM_RENDER_MODES];

RENDER_MODE currentRenderMode = TILED_DEFERRED;

std::chrono::time_point<std::chrono::high_resolution_clock> start, current;
float currentTime;


vec3 lightGridOffset = vec3(0.0f, 0.6f, 0.0f);
vec2 lightGridScale = vec2(0.7f); //(3.5f);
bool dynamicLights = true;

GLFWwindow* window;

Renderer renderer;

Material material;

int Init();
void InitNoRendering();
void InitForwardSimpleRendering();
void InitDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void InitTiledDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer);
void ImGuiRender();

int main()
{
	static_assert(!((g_WindowWidth % g_TileSize) | (g_WindowHeight % g_TileSize)));

	start = std::chrono::high_resolution_clock::now();

	int initResult = Init();
	if (initResult)
		return initResult;

	{
		renderer.camera.projectionMatrix = mat4::Perspective(g_FOV, float(g_WindowWidth) / float(g_WindowHeight), g_NearPlaneDepth, g_FarPlaneDepth);
		renderer.camera.position = vec3(0.0f, 4.0f, 12.0f);
		renderer.camera.rotation = vec3(-0.4f, 0.0f, 0.0f);
		mat4 V = renderer.camera.GetViewMatrix();

		Cube cubeGrid[g_CubeGridSize][g_CubeGridSize];
		for (int x = 0; x < g_CubeGridSize; x++)
			for (int z = 0; z < g_CubeGridSize; z++)
				cubeGrid[x][z].position = vec3(2.0f * (x - 4.0f), 0.0f, 2.0f * (z - 4.0f));
		PlaneMesh planeMesh({ 0.0f, -0.5f, 0.0f }, { 24.0f, 24.0f });

		for (int x = 0; x < g_LightGridSize; x++)
		{
			for (int z = 0; z < g_LightGridSize; z++)
			{
				g_LightGrid[x][z].color = vec4(g_LightIntensityMultiplier, g_LightIntensityMultiplier, g_LightIntensityMultiplier, 1.0f);
				vec4 position = vec4(lightGridScale.x * (x - (g_LightGridSize - 1) / 2) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - (g_LightGridSize - 1) / 2) + lightGridOffset.z, 1.0f);
				g_LightGrid[x][z].viewSpacePosition = V * position;
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

		/// Render techniques
		{
			// Timers
			std::shared_ptr<GLTimer> prepassTimer = std::make_shared<GLTimer>();
			std::shared_ptr<GLTimer> lightingPassTimer = std::make_shared<GLTimer>();
			std::shared_ptr<GLTimer> totalRenderTimer = std::make_shared<GLTimer>();

			// No rendering
			InitNoRendering();

			// Forward non-tiled rendering
			InitForwardSimpleRendering();

			// Deferred non-tiled rendering
			InitDeferredRendering(prepassTimer, lightingPassTimer, totalRenderTimer);

			// Tiled deferred rendering
			InitTiledDeferredRendering(prepassTimer, lightingPassTimer, totalRenderTimer);
		}
		while (!glfwWindowShouldClose(window))
		{
			current = std::chrono::high_resolution_clock::now();
			currentTime = std::chrono::duration<float>(current - start).count();

			float c = cos(currentTime);
			float s = sin(currentTime);

			glfwPollEvents();

#ifdef USE_IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif

			if (dynamicLights)
			{
				for (int x = 0; x < g_LightGridSize; x++)
				{
					for (int z = 0; z < g_LightGridSize; z++)
					{
						g_LightGrid[x][z].color = vec4(g_LightIntensityMultiplier, g_LightIntensityMultiplier, g_LightIntensityMultiplier, 1.0f);
						vec4 position = vec4(0.5f * (s - 1.0f) * lightGridScale.x * (x - (g_LightGridSize - 1) / 2) + lightGridOffset.x, lightGridOffset.y, 0.5f * (s - 1.0f) * lightGridScale.y * (z - (g_LightGridSize - 1) / 2) + lightGridOffset.z, 1.0f);
						g_LightGrid[x][z].viewSpacePosition = V * position;
					}
				}
			}
			else
			{
				for (int x = 0; x < g_LightGridSize; x++)
				{
					for (int z = 0; z < g_LightGridSize; z++)
					{
						g_LightGrid[x][z].color = vec4(g_LightIntensityMultiplier, g_LightIntensityMultiplier, g_LightIntensityMultiplier, 1.0f);
						vec4 position = vec4(lightGridScale.x * (x - (g_LightGridSize - 1) / 2) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - (g_LightGridSize - 1) / 2) + lightGridOffset.z, 1.0f);
						g_LightGrid[x][z].viewSpacePosition = V * position;
					}
				}
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

	ImGui::Separator();

	ImGui::Text("Light");
	ImGui::Checkbox("Dynamic lights", &dynamicLights);
	ImGui::SliderFloat("Light intensity multiplier", &g_LightIntensityMultiplier, 0.0f, 100.0f, "%.2f", 3.0f);
	ImGui::SliderFloat("Light falloff threshold", &g_LightFalloffThreshold, 0.001f, 0.1f, "%.3f", 2.0f);
	ImGui::SliderFloat3("Light grid offset", &lightGridOffset.x, -3.0f, 3.0f, "%.1f", 1.0f);
	ImGui::SliderFloat2("Light grid scale", &lightGridScale.x, 0.0f, 10.0f, "%.1f", 1.0f);

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

void InitForwardSimpleRendering()
{
	std::shared_ptr<GLShader> forwardPrepassShader = std::make_shared<GLShader>();
	forwardPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/forward/forward_prepass_vs.glsl");
	forwardPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/forward/forward_prepass_fs.glsl");
	forwardPrepassShader->CompileShaders();

	std::shared_ptr<ForwardPrepass> forwardPrepass = std::make_shared<ForwardPrepass>(
		renderer,
		forwardPrepassShader
	);

	std::shared_ptr<RenderTechnique> forwardRendering = std::make_shared<RenderTechnique>();
	forwardRendering->AddRenderPass(forwardPrepass);

	renderModes[FORWARD].first = forwardRendering;
	renderModes[FORWARD].second = "Forward rendering";
}

void InitDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
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
	std::shared_ptr<StartGLTimerPass> startPrepassRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopGLTimerPass> stopPrepassRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartGLTimerPass> startLightingPassRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopGLTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartGLTimerPass> startTotalRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopGLTimerPass> stopTotalRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> deferredRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	deferredRendering->AddRenderPass(plotTimersPass);
	deferredRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	deferredRendering->AddRenderPass(startPrepassRenderTimePass);
	deferredRendering->AddRenderPass(deferredPrepass);
	deferredRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Lighting
	deferredRendering->AddRenderPass(startLightingPassRenderTimePass);
	deferredRendering->AddRenderPass(deferredLightingPass);
	deferredRendering->AddRenderPass(stopLightingPassRenderTimePass);
	deferredRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[DEFERRED].first = deferredRendering;
	renderModes[DEFERRED].second = "Deferred rendering";
}

void InitTiledDeferredRendering(std::shared_ptr<GLTimer> prepassTimer, std::shared_ptr<GLTimer> lightingPassTimer, std::shared_ptr<GLTimer> totalRenderTimer)
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

	std::shared_ptr<GLImageTexture2D> tileMinDepthImageTexture = std::make_shared<GLImageTexture2D>(GL_R32I, GL_READ_WRITE, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize);
	tileMinDepthImageTexture->SetMinMagFilter(GL_NEAREST);
	tileMinDepthImageTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLImageTexture2D> tileMaxDepthImageTexture = std::make_shared<GLImageTexture2D>(GL_R32I, GL_READ_WRITE, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize);
	tileMaxDepthImageTexture->SetMinMagFilter(GL_NEAREST);
	tileMaxDepthImageTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	// SSBOs
	std::shared_ptr<GLShaderStorageBuffer> lightIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, 0);
	std::shared_ptr<GLShaderStorageBuffer> tileIndexSSBO = std::make_shared<GLShaderStorageBuffer>(nullptr, 0);

	// Shaders
	std::shared_ptr<GLShader> tiledDeferredClearTileMinMaxDepthShader = std::make_shared<GLShader>();
	tiledDeferredClearTileMinMaxDepthShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/tiled/tiled_deferred_clear_tile_min_max_depth_vs.glsl");
	tiledDeferredClearTileMinMaxDepthShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/tiled/tiled_deferred_clear_tile_min_max_depth_fs.glsl");
	tiledDeferredClearTileMinMaxDepthShader->CompileShaders();

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

	// Render passes
	std::shared_ptr<TiledDeferredClearTileMinMaxDepthPass> tiledDeferredClearTileMinMaxDepthPass = std::make_shared<TiledDeferredClearTileMinMaxDepthPass>(
		renderer,
		tiledDeferredClearTileMinMaxDepthShader,
		tileMinDepthImageTexture,
		tileMaxDepthImageTexture
	);

	std::shared_ptr<TiledDeferredPrepass> tiledDeferredPrepass = std::make_shared<TiledDeferredPrepass>(
		renderer,
		tiledDeferredPrepassShader,
		viewSpacePositionTexture,
		viewSpaceNormalTexture,
		tileMinDepthImageTexture,
		tileMaxDepthImageTexture
	);

	std::shared_ptr<TiledDeferredComputeLightTilesPass> tiledDeferredComputeLightTilesPass = std::make_shared<TiledDeferredComputeLightTilesPass>(
		renderer,
		tileMinDepthImageTexture,
		tileMaxDepthImageTexture,
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
	std::shared_ptr<StartGLTimerPass> startPrepassRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, prepassTimer);
	std::shared_ptr<StopGLTimerPass> stopPrepassRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, prepassTimer);

	std::shared_ptr<StartGLTimerPass> startLightingPassRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, lightingPassTimer);
	std::shared_ptr<StopGLTimerPass> stopLightingPassRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, lightingPassTimer);

	std::shared_ptr<StartGLTimerPass> startTotalRenderTimePass = std::make_shared<StartGLTimerPass>(renderer, totalRenderTimer);
	std::shared_ptr<StopGLTimerPass> stopTotalRenderTimePass = std::make_shared<StopGLTimerPass>(renderer, totalRenderTimer);

	std::shared_ptr<PlotTimersPass> plotTimersPass = std::make_shared<PlotTimersPass>(renderer);
	plotTimersPass->AddTimer("Total render time", totalRenderTimer);
	plotTimersPass->AddTimer("Prepass render time", prepassTimer);
	plotTimersPass->AddTimer("Lighting pass render time", lightingPassTimer);

	// Create RenderTechnique
	std::shared_ptr<RenderTechnique> tiledDeferredRendering = std::make_shared<RenderTechnique>();
	// Plot render times
	tiledDeferredRendering->AddRenderPass(plotTimersPass);
	tiledDeferredRendering->AddRenderPass(startTotalRenderTimePass);
	// Prepass
	tiledDeferredRendering->AddRenderPass(startPrepassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredClearTileMinMaxDepthPass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredPrepass);
	tiledDeferredRendering->AddRenderPass(stopPrepassRenderTimePass);
	// Compute light tiles
	tiledDeferredRendering->AddRenderPass(tiledDeferredComputeLightTilesPass);
	// Lighting
	tiledDeferredRendering->AddRenderPass(startLightingPassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(tiledDeferredLightingPass);
	tiledDeferredRendering->AddRenderPass(stopLightingPassRenderTimePass);
	tiledDeferredRendering->AddRenderPass(stopTotalRenderTimePass);

	renderModes[TILED_DEFERRED].first = tiledDeferredRendering;
	renderModes[TILED_DEFERRED].second = "Tiled deferred rendering";
}