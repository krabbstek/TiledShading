#include "Globals.h"
#include "Plane.h"
#include "graphics/Light.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/opengl/OpenGL.h"
#include "math/math.h"
#include "meshes/Cube.h"
#include "meshes/PlaneMesh.h"
#include "graphics/RenderTechnique.h"
#include "graphics/renderpasses/forward/ForwardPrepass.h"
#include "graphics/renderpasses/deferred/DeferredPrepass.h"
#include "graphics/renderpasses/deferred/DeferredLightingPass.h"

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
	DEPTH_ONLY,
	TILE_MIN_DEPTH,
	TILE_MAX_DEPTH,
	NON_TILED_DEFERRED,
	TILED_DEFERRED,
};

std::chrono::time_point<std::chrono::high_resolution_clock> start, current;
float currentTime;

std::vector<Plane> leftPlanes, rightPlanes, bottomPlanes, topPlanes;
std::vector<std::pair<unsigned int, unsigned int>> tileLights[tileRows][tileCols];
std::vector<int> lightIndices;
int tileLightIndices[2 * g_WindowWidth * g_WindowHeight / (g_TileSize * g_TileSize)];

RENDER_MODE renderMode = TILED_DEFERRED;

float fullscreenVertices[] =
{
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	 1.0f,  1.0f,
	-1.0f,  1.0f,
};
unsigned int fullscreenIndices[] =
{
	0, 1, 2,
	0, 2, 3,
};

vec3 lightGridOffset = vec3(0.0f, 0.6f, 0.0f);
vec2 lightGridScale = vec2(0.7f); //(3.5f);
float lightIntensity = 0.1f; // 3.0f;
float lightRadiusMultiplier = 1.0f;
bool dynamicLights = true;

GLFWwindow* window;

Renderer renderer;

/// Render techniques
std::shared_ptr<RenderTechnique> g_ForwardRendering;
std::shared_ptr<RenderTechnique> g_DeferredRendering;

/// Framebuffers
GLuint defaultFramebuffer = 0;
GLuint prepassFramebuffer;
GLuint tileFramebuffer;

/// Depthbuffer
GLuint prepassDepthbuffer;

/// Textures
GLTexture2D* prepassDepthTexture;
GLTexture2D* prepassAlbedoTexture;
GLTexture2D* prepassViewSpacePositionTexture;
GLTexture2D* prepassViewSpaceNormalTexture;
GLTexture2D* tileMinTexture;
GLTexture2D* tileMaxTexture;
GLTexture2D* rainbowTexture;

/// Shaders
GLShader* deferredShader;
GLShader* fullscreenShader;
GLShader* prepassShader;
GLShader* lightingNonTiledShader;
GLShader* lightingTiledShader;
GLShader* depthShader;
GLShader* simpleShader;
GLShader* tileDepthShader;
GLShader* tileMinMaxShader;

/// Buffer objects
GLVertexBuffer* fullscreenVBO;
GLVertexArray* fullscreenVAO;
GLIndexBuffer* fullscreenIBO;
GLShaderStorageBuffer* lightSSBO;
GLShaderStorageBuffer* lightIndexSSBO;
GLShaderStorageBuffer* tileIndexSSBO;

mat4 projectionMatrix;
mat4 viewMatrix;
mat4 modelMatrix;

Material material;

int Init();
void InitDeferredRendering();
void RenderDepthOnly(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh);
void RenderTileMinDepth(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh);
void RenderTileMaxDepth(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh);
void RenderNonTiledDeferred(Cube (&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh);
void RenderTiledDeferred(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh);
void ImGuiRender();

int main()
{
	static_assert(!((g_WindowWidth % g_TileSize) | (g_WindowHeight % g_TileSize)));

	start = std::chrono::high_resolution_clock::now();

	int initResult = Init();
	if (initResult)
		return initResult;

	float fovPerTile = g_FOV / float(tileRows);
	float halfFovPerTile = 0.5f * fovPerTile;

	leftPlanes.push_back(Plane(cos(halfFovPerTile * -tileCols), 0.0f, sin(halfFovPerTile * -tileCols), 0.0f));
	for (int col = -(tileCols - 2); col <= (tileCols - 2); col += 2)
	{
		float c = cos(col * halfFovPerTile);
		float s = sin(col * halfFovPerTile);

		vec3 normal;
		normal.x = c;
		normal.z = s;

		leftPlanes.push_back(Plane(normal, 0));
		rightPlanes.push_back(Plane(-normal, 0));
	}
	rightPlanes.push_back(Plane(-cos(halfFovPerTile * tileCols), 0.0f, -sin(halfFovPerTile * tileCols), 0.0f));

	bottomPlanes.push_back(Plane(vec3(0.0f, cos(halfFovPerTile * -tileCols), sin(halfFovPerTile * -tileCols)), 0));
	for (int row = -(tileRows - 2); row <= tileRows - 2; row += 2)
	{
		float c = cos(row * halfFovPerTile);
		float s = sin(row * halfFovPerTile);

		vec3 normal;
		normal.y = c;
		normal.z = s;

		bottomPlanes.push_back(Plane(normal, 0));
		topPlanes.push_back(Plane(-normal, 0));
	}
	topPlanes.push_back(Plane(vec3(0.0f, -cos(halfFovPerTile * tileCols), -sin(halfFovPerTile * tileCols)), 0));

	{
		renderer.camera.projectionMatrix = mat4::Perspective(g_FOV, float(g_WindowWidth) / float(g_WindowHeight), 0.1f, 100.0f);
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
				g_LightGrid[x][z].color = vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
				vec4 position = vec4(lightGridScale.x * (x - (g_LightGridSize - 1) / 2) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - (g_LightGridSize - 1) / 2) + lightGridOffset.z, 1.0f);
				g_LightGrid[x][z].viewSpacePosition = V * position;
			}
		}
		lightSSBO = new GLShaderStorageBuffer(g_LightGrid, sizeof(g_LightGrid));
		lightIndexSSBO = new GLShaderStorageBuffer(0, 0);
		tileIndexSSBO = new GLShaderStorageBuffer(0, 0);

		material.albedo = vec4(0.2, 0.3, 0.8);
		material.reflectivity = 1.0;
		material.shininess = 1.0;
		material.metalness = 0.5;
		material.fresnel = 0.5;

		GLVertexBufferLayout fullScreenLayout;
		fullScreenLayout.Push(GL_FLOAT, 2);
		fullscreenVBO = new GLVertexBuffer(fullscreenVertices, sizeof(fullscreenVertices));
		fullscreenVBO->SetVertexBufferLayout(fullScreenLayout);

		fullscreenVAO = new GLVertexArray();
		fullscreenVAO->AddVertexBuffer(*fullscreenVBO);

		fullscreenIBO = new GLIndexBuffer(fullscreenIndices, sizeof(fullscreenIndices) / sizeof(unsigned int));

		/// Textures
		prepassDepthTexture = new GLTexture2D();
		prepassDepthTexture->Load(GL_R32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RED, GL_FLOAT);
		prepassDepthTexture->SetMinMagFilter(GL_LINEAR);
		prepassDepthTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassAlbedoTexture = new GLTexture2D();
		prepassAlbedoTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_FLOAT);
		prepassAlbedoTexture->SetMinMagFilter(GL_LINEAR);
		prepassAlbedoTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassViewSpacePositionTexture = new GLTexture2D();
		prepassViewSpacePositionTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_FLOAT);
		prepassViewSpacePositionTexture->SetMinMagFilter(GL_LINEAR);
		prepassViewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassViewSpaceNormalTexture = new GLTexture2D();
		prepassViewSpaceNormalTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_FLOAT);
		prepassViewSpaceNormalTexture->SetMinMagFilter(GL_LINEAR);
		prepassViewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		tileMinTexture = new GLTexture2D();
		tileMinTexture->Load(GL_R32F, nullptr, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize, GL_RED, GL_FLOAT);
		tileMinTexture->SetMinMagFilter(GL_LINEAR);
		tileMinTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		tileMaxTexture = new GLTexture2D();
		tileMaxTexture->Load(GL_R32F, nullptr, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize, GL_RED, GL_FLOAT);
		tileMaxTexture->SetMinMagFilter(GL_LINEAR);
		tileMaxTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		rainbowTexture = new GLTexture2D();
		rainbowTexture->LoadFromFile("res/textures/rainbow_small.jpg");
		rainbowTexture->SetMinMagFilter(GL_LINEAR);
		rainbowTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		/// Depth buffer
		GLCall(glGenRenderbuffers(1, &prepassDepthbuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, prepassDepthbuffer));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, g_WindowWidth, g_WindowHeight));

		/// Framebuffers
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glFrontFace(GL_CCW));
		GLCall(glCullFace(GL_BACK));
		GLCall(glDepthFunc(GL_LEQUAL));
		
		GLCall(glGenFramebuffers(1, &prepassFramebuffer));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, prepassDepthbuffer));
		GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, prepassDepthbuffer));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, prepassAlbedoTexture->RendererID(), 0));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, prepassViewSpacePositionTexture->RendererID(), 0));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, prepassViewSpaceNormalTexture->RendererID(), 0));
		GLenum attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		GLCall(glDrawBuffers(3, attachments));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glFrontFace(GL_CCW));
		GLCall(glCullFace(GL_BACK));
		GLCall(glDepthFunc(GL_LEQUAL));

		GLCall(glGenFramebuffers(1, &tileFramebuffer));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, tileFramebuffer));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tileMinTexture->RendererID(), 0));
		GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tileMaxTexture->RendererID(), 0));
		GLCall(glDrawBuffers(2, attachments));

		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));

		/// Shaders
		deferredShader = new GLShader();
		deferredShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred_vs.glsl");
		deferredShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred_fs.glsl");
		deferredShader->CompileShaders();

		fullscreenShader = new GLShader();
		fullscreenShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/fullscreen_vs.glsl");
		fullscreenShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/fullscreen_fs.glsl");
		fullscreenShader->CompileShaders();

		prepassShader = new GLShader();
		prepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/prepass_vs.glsl");
		prepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/prepass_fs.glsl");
		prepassShader->CompileShaders();

		lightingNonTiledShader = new GLShader();
		lightingNonTiledShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/lighting_nontiled_vs.glsl");
		lightingNonTiledShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/lighting_nontiled_fs.glsl");
		lightingNonTiledShader->CompileShaders();

		lightingTiledShader = new GLShader();
		lightingTiledShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/lighting_tiled_vs.glsl");
		lightingTiledShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/lighting_tiled_fs.glsl");
		lightingTiledShader->CompileShaders();

		depthShader = new GLShader();
		depthShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/depth_vs.glsl");
		depthShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/depth_fs.glsl");
		depthShader->CompileShaders();

		tileDepthShader = new GLShader();
		tileDepthShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/tile_depth_vs.glsl");
		tileDepthShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/tile_depth_fs.glsl");
		tileDepthShader->CompileShaders();

		tileMinMaxShader = new GLShader();
		tileMinMaxShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/tile_min_max_vs.glsl");
		tileMinMaxShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/tile_min_max_fs.glsl");
		tileMinMaxShader->CompileShaders();

		simpleShader = new GLShader();
		simpleShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/simple_vs.glsl");
		simpleShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/simple_fs.glsl");
		simpleShader->CompileShaders();

		std::shared_ptr<GLShader> m_SimpleShader = std::make_shared<GLShader>();
		m_SimpleShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/simple_vs.glsl");
		m_SimpleShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/simple_fs.glsl");
		m_SimpleShader->CompileShaders();

		/// Render techniques
		{
			// Forward non-tiled rendering
			g_ForwardRendering = std::make_shared<RenderTechnique>();
			g_ForwardRendering->AddRenderPass(std::make_shared<ForwardPrepass>(renderer, m_SimpleShader));

			InitDeferredRendering();
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
						g_LightGrid[x][z].color = vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
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
						g_LightGrid[x][z].color = vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
						vec4 position = vec4(lightGridScale.x * (x - (g_LightGridSize - 1) / 2) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - (g_LightGridSize - 1) / 2) + lightGridOffset.z, 1.0f);
						g_LightGrid[x][z].viewSpacePosition = V * position;
					}
				}
			}
			lightSSBO->SetData(g_LightGrid, sizeof(g_LightGrid));

			// Render
			//g_CurrentRenderTechnique->Render();

			/*for (int x = 0; x < CUBE_GRID_SIZE; x++)
				for (int y = 0; y < CUBE_GRID_SIZE; y++)
					g_ForwardRendering.Render(cubeGrid[x][y]);
			g_ForwardRendering.Render(planeMesh);
			g_ForwardRendering.Render();*/

			g_DeferredRendering->Render(planeMesh);
			g_DeferredRendering->Render();

#if 0
			switch (renderMode)
			{
			case NONE:
				GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
				GLCall(glClearColor(0.2f, 0.3f, 0.8f, 1.0f));
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
				break;

			case DEPTH_ONLY:
				RenderDepthOnly(cubeGrid, planeMesh);
				break;

			case TILE_MIN_DEPTH:
				RenderTileMinDepth(cubeGrid, planeMesh);
				break;

			case TILE_MAX_DEPTH:
				RenderTileMaxDepth(cubeGrid, planeMesh);
				break;

			case NON_TILED_DEFERRED:
				RenderNonTiledDeferred(cubeGrid, planeMesh);
				break;

			case TILED_DEFERRED:
				RenderTiledDeferred(cubeGrid, planeMesh);
				break;
			}
#endif
			
#ifdef USE_IMGUI
			ImGuiRender();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
			
			glfwSwapBuffers(window);
		}
	}

	g_ForwardRendering.reset();
	g_DeferredRendering.reset();

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
	glfwSwapInterval(1);

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
	ImGui::Begin("Config");

	ImGui::Text("Render mode");
	ImGui::RadioButton("None", (int*)&renderMode, (int)NONE);
	ImGui::RadioButton("Depth only", (int*)&renderMode, (int)DEPTH_ONLY);
	ImGui::RadioButton("Tile min depth", (int*)&renderMode, (int)TILE_MIN_DEPTH);
	ImGui::RadioButton("Tile max depth", (int*)&renderMode, (int)TILE_MAX_DEPTH);
	ImGui::RadioButton("Non-tiled, deferred", (int*)&renderMode, (int)NON_TILED_DEFERRED);
	ImGui::RadioButton("Tiled, deferred", (int*)&renderMode, (int)TILED_DEFERRED);

	ImGui::Separator();

	ImGui::Text("Light");
	ImGui::Checkbox("Dynamic lights", &dynamicLights);
	ImGui::SliderFloat("Light intensity", &lightIntensity, 0.0f, 100.0f, "%.2f", 3.0f);
	ImGui::SliderFloat("Light radius multiplier", &lightRadiusMultiplier, 0.0f, 100.0f, "%.1f", 3.0f);
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


void RenderDepthOnly(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *depthShader);

	planeMesh.Render(renderer, *depthShader);
}

void RenderTileMinDepth(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh)
{
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	prepassShader->SetUniform1i("u_TileSize", g_TileSize);

	GLCall(glBindImageTexture(0, tileMinTexture->RendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
	GLCall(glBindImageTexture(1, tileMaxTexture->RendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

	for (int x = 0; x < 9; x++)
	{
		for (int z = 0; z < 9; z++)
		{
			cubeGrid[x][z].Render(renderer, *prepassShader);
			GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
		}
	}
	planeMesh.Render(renderer, *prepassShader);

	GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

	GLCall(glViewport(0, 0, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, tileFramebuffer));
	tileMinMaxShader->Bind();
	tileMinMaxShader->SetUniform1i("u_TileSize", g_TileSize);
	prepassViewSpacePositionTexture->Bind();
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));

	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClearColor(0.8f, 0.2f, 0.2f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	tileMinTexture->Bind();
	tileDepthShader->Bind();
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));
}

void RenderTileMaxDepth(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh)
{
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	prepassShader->SetUniform1i("u_TileSize", g_TileSize);

	GLCall(glBindImageTexture(0, tileMinTexture->RendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));
	GLCall(glBindImageTexture(1, tileMaxTexture->RendererID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32UI));

	for (int x = 0; x < 9; x++)
	{
		for (int z = 0; z < 9; z++)
		{
			cubeGrid[x][z].Render(renderer, *prepassShader);
			GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));
		}
	}
	planeMesh.Render(renderer, *prepassShader);

	GLCall(glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT));

	GLCall(glViewport(0, 0, g_WindowWidth / g_TileSize, g_WindowHeight / g_TileSize));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, tileFramebuffer));
	tileMinMaxShader->Bind();
	tileMinMaxShader->SetUniform1i("u_TileSize", g_TileSize);
	prepassViewSpacePositionTexture->Bind();
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));

	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClearColor(0.8f, 0.2f, 0.2f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	tileMaxTexture->Bind();
	tileDepthShader->Bind();
	tileDepthShader->SetUniform1i("u_TileSize", g_TileSize);
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));
}

void RenderNonTiledDeferred(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *prepassShader);
	planeMesh.Render(renderer, *prepassShader);

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	lightingNonTiledShader->Bind();
	lightingNonTiledShader->SetUniform4f("u_Material.albedo", material.albedo);
	lightingNonTiledShader->SetUniform1f("u_Material.reflectivity", material.reflectivity);
	lightingNonTiledShader->SetUniform1f("u_Material.shininess", material.shininess);
	lightingNonTiledShader->SetUniform1f("u_Material.metalness", material.metalness);
	lightingNonTiledShader->SetUniform1f("u_Material.fresnel", material.fresnel);
	prepassAlbedoTexture->Bind(0);
	prepassViewSpacePositionTexture->Bind(1);
	prepassViewSpaceNormalTexture->Bind(2);
	GLCall(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO->RendererID(), 0, lightSSBO->Size()));
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));
}

void RenderTiledDeferred(Cube(&cubeGrid)[g_CubeGridSize][g_CubeGridSize], const PlaneMesh& planeMesh)
{
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *prepassShader);
	planeMesh.Render(renderer, *prepassShader);

	float lightRadius = lightRadiusMultiplier * 10.0f * sqrt(lightIntensity);

	float tileMin[g_WindowWidth * g_WindowHeight / (g_TileSize * g_TileSize)];
	float tileMax[g_WindowWidth * g_WindowHeight / (g_TileSize * g_TileSize)];

	tileMinTexture->Bind();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, tileMin));
	tileMaxTexture->Bind();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, tileMax));

	for (int col = 0; col < tileCols; col++)
		for (int row = 0; row < tileRows; row++)
			tileLights[row][col].clear();

	for (int lightX = 0; lightX < g_LightGridSize; lightX++)
	{
		for (int lightZ = 0; lightZ < g_LightGridSize; lightZ++)
		{
			for (int col = 0; col < tileCols; col++)
			{
				const vec3& viewSpacePosition = g_LightGrid[lightX][lightZ].viewSpacePosition;

				float dLeft = leftPlanes[col].Distance(viewSpacePosition);
				if (dLeft < -lightRadius)
					continue;
				float dRight = rightPlanes[col].Distance(viewSpacePosition);
				if (dRight < -lightRadius)
					continue;

				for (int row = 0; row < tileRows; row++)
				{
					float dBottom = bottomPlanes[row].Distance(viewSpacePosition);
					if (dBottom < -lightRadius)
						continue;
					float dTop = topPlanes[row].Distance(viewSpacePosition);
					if (dTop < -lightRadius)
						continue;

					float dNear = viewSpacePosition.z - tileMax[row * (g_WindowWidth / g_TileSize) + col];
					if (dNear > lightRadius)
						continue;

					float dFar = viewSpacePosition.z - tileMin[row * (g_WindowWidth / g_TileSize) + col];
					if (dFar > lightRadius)
						continue;

					tileLights[row][col].emplace_back(lightX, lightZ);
				}
			}
		}
	}

	lightIndices.clear();

	for (int row = 0; row < tileRows; row++)
	{
		for (int col = 0; col < tileCols; col++)
		{
			tileLightIndices[2 * (tileCols * row + col)] = lightIndices.size();
			int size = tileLights[row][col].size();
			tileLightIndices[2 * (tileCols * row + col) + 1] = size;
			for (int i = 0; i < size; i++)
			{
				std::pair<int, int> ind = tileLights[row][col][i];
				lightIndices.emplace_back(ind.first * g_LightGridSize + ind.second);
			}
		}
	}
	if (!lightIndices.size())
		lightIndices.push_back(0);
	lightIndexSSBO->SetData(lightIndices.data(), lightIndices.size() * sizeof(int));
	tileIndexSSBO->SetData(tileLightIndices, sizeof(tileLightIndices));

	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	lightingTiledShader->Bind();
	lightingTiledShader->SetUniform4f("u_Material.albedo", material.albedo);
	lightingTiledShader->SetUniform1f("u_Material.reflectivity", material.reflectivity);
	lightingTiledShader->SetUniform1f("u_Material.shininess", material.shininess);
	lightingTiledShader->SetUniform1f("u_Material.metalness", material.metalness);
	lightingTiledShader->SetUniform1f("u_Material.fresnel", material.fresnel);
	lightingTiledShader->SetUniform1i("u_TileSize", g_TileSize);
	lightingTiledShader->SetUniform1i("u_NumTileCols", tileCols);

	prepassAlbedoTexture->Bind(0);
	prepassViewSpacePositionTexture->Bind(1);
	prepassViewSpaceNormalTexture->Bind(2);
	GLCall(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO->RendererID(), 0, lightSSBO->Size()));
	GLCall(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 4, lightIndexSSBO->RendererID(), 0, lightIndexSSBO->Size()));
	GLCall(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 5, tileIndexSSBO->RendererID(), 0, tileIndexSSBO->Size()));
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));
}


void InitDeferredRendering()
{
	std::shared_ptr<GLTexture2D> viewSpacePositionTexture = std::make_shared<GLTexture2D>();
	viewSpacePositionTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpacePositionTexture->SetMinMagFilter(GL_NEAREST);
	viewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLTexture2D> viewSpaceNormalTexture = std::make_shared<GLTexture2D>();
	viewSpaceNormalTexture->Load(GL_RGB32F, nullptr, g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE);
	viewSpaceNormalTexture->SetMinMagFilter(GL_NEAREST);
	viewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

	std::shared_ptr<GLShader> deferredPrepassShader = std::make_shared<GLShader>();
	deferredPrepassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/deferred_prepass_vs.glsl");
	deferredPrepassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/deferred_prepass_fs.glsl");
	deferredPrepassShader->CompileShaders();

	std::shared_ptr<GLShader> deferredLightingPassShader = std::make_shared<GLShader>();
	deferredLightingPassShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/deferred/deferred_lighting_pass_vs.glsl");
	deferredLightingPassShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/deferred/deferred_lighting_pass_fs.glsl");
	deferredLightingPassShader->CompileShaders();

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

	g_DeferredRendering = std::make_shared<RenderTechnique>();
	g_DeferredRendering->AddRenderPass(deferredPrepass);
	g_DeferredRendering->AddRenderPass(deferredLightingPass);
}