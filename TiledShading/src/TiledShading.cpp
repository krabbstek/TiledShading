#include "Cube.h"
#include "graphics/Light.h"
#include "graphics/Material.h"
#include "graphics/Renderer.h"
#include "graphics/opengl/OpenGL.h"
#include "math/math.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#define USE_IMGUI

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;
constexpr unsigned int TILE_SIZE = 40;

constexpr unsigned int CUBE_GRID_SIZE = 9;

enum RENDER_MODE
{
	NONE = 0,
	DEPTH_ONLY,
	NON_TILED_DEFERRED,
};

RENDER_MODE renderMode = NON_TILED_DEFERRED;

Light lights[7][7];

float fullscreenVertices[] =
{
	-1.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	-1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
};
unsigned int fullscreenIndices[] =
{
	0, 1, 2,
	0, 2, 3,
};

float planeVertices[] =
{
	-12.0f, -0.5f, -12.0f,  0.0f, 1.0f, 0.0f,  0.67f, 0.94f, 0.36f,
	 12.0f, -0.5f, -12.0f,  0.0f, 1.0f, 0.0f,  0.67f, 0.94f, 0.36f,
	 12.0f, -0.5f,  12.0f,  0.0f, 1.0f, 0.0f,  0.67f, 0.94f, 0.36f,
	-12.0f, -0.5f,  12.0f,  0.0f, 1.0f, 0.0f,  0.67f, 0.94f, 0.36f,
};
unsigned int planeIndices[] =
{
	0, 1, 2,
	0, 2, 3,
};

vec3 lightGridOffset;
vec2 lightGridScale;
float lightIntensity = 50.0f;

GLFWwindow* window;

Renderer renderer;

/// Framebuffers
GLuint defaultFramebuffer = 0;
GLuint prepassFramebuffer;
GLuint tileMinMaxFramebuffer;

/// Depthbuffer
GLuint prepassDepthbuffer;

/// Textures
GLTexture2D* prepassDepthTexture;
GLTexture2D* prepassAlbedoTexture;
GLTexture2D* prepassViewSpacePositionTexture;
GLTexture2D* prepassViewSpaceNormalTexture;
GLTexture2D* tileMinMaxTexture;

/// Shaders
GLShader* deferredShader;
GLShader* fullscreenShader;
GLShader* prepassShader;
GLShader* lightingNonTiledShader;
GLShader* depthShader;

/// Buffer objects
GLVertexBuffer* fullscreenVBO;
GLVertexArray* fullscreenVAO;
GLIndexBuffer* fullscreenIBO;
GLShaderStorageBuffer* lightSSBO;
GLVertexBuffer* planeVBO;
GLVertexArray* planeVAO;
GLIndexBuffer* planeIBO;

mat4 projectionMatrix;
mat4 viewMatrix;
mat4 modelMatrix;

Material material;

int Init();
void RenderDepthOnly(Cube(&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE]);
void RenderNonTiledDeferred(Cube (&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE]);
void RenderTiledDeferred(Cube(&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE]);
void ImGuiRender();

int main()
{
	static_assert(!((WINDOW_WIDTH % TILE_SIZE) | (WINDOW_HEIGHT % TILE_SIZE)));

	int initResult = Init();
	if (initResult)
		return initResult;

	{
		renderer.camera.projectionMatrix = mat4::Perspective(DegToRad(75.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
		renderer.camera.position = vec3(0.0f, 4.0f, 12.0f);
		renderer.camera.rotation = vec3(-0.4f, 0.0f, 0.0f);
		mat4 V = renderer.camera.GetViewMatrix();

		Cube cubeGrid[CUBE_GRID_SIZE][CUBE_GRID_SIZE];
		for (int x = 0; x < CUBE_GRID_SIZE; x++)
			for (int z = 0; z < CUBE_GRID_SIZE; z++)
				cubeGrid[x][z].position = vec3(2.0f * (x - 4.0f), 0.0f, 2.0f * (z - 4.0f));
		planeVBO = new GLVertexBuffer(planeVertices, sizeof(planeVertices));
		GLVertexBufferLayout planeLayout;
		planeLayout.Push(GL_FLOAT, 3);
		planeLayout.Push(GL_FLOAT, 3);
		planeLayout.Push(GL_FLOAT, 3);
		planeVBO->SetVertexBufferLayout(planeLayout);

		for (int x = 0; x < 7; x++)
		{
			for (int z = 0; z < 7; z++)
			{
				lights[x][z].color = vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
				vec4 position = vec4(lightGridScale.x * (x - 3) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - 3) + lightGridOffset.z, 1.0f);
				lights[x][z].viewSpacePosition = V * position;
			}
		}
		lightSSBO = new GLShaderStorageBuffer(lights, sizeof(lights));

		material.albedo = vec4(0.2, 0.3, 0.8);
		material.reflectivity = 1.0;
		material.shininess = 1.0;
		material.metalness = 0.5;
		material.fresnel = 0.5;

		planeVAO = new GLVertexArray();
		planeVAO->AddVertexBuffer(*planeVBO);

		planeIBO = new GLIndexBuffer(planeIndices, sizeof(planeIndices) / sizeof(unsigned int));

		GLVertexBufferLayout layout;
		layout.Push(GL_FLOAT, 3);
		layout.Push(GL_FLOAT, 2);
		fullscreenVBO = new GLVertexBuffer(fullscreenVertices, sizeof(fullscreenVertices));
		fullscreenVBO->SetVertexBufferLayout(layout);

		fullscreenVAO = new GLVertexArray();
		fullscreenVAO->AddVertexBuffer(*fullscreenVBO);

		fullscreenIBO = new GLIndexBuffer(fullscreenIndices, sizeof(fullscreenIndices) / sizeof(unsigned int));

		/// Textures
		prepassDepthTexture = new GLTexture2D();
		prepassDepthTexture->Load(GL_R32F, nullptr, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RED, GL_FLOAT);
		prepassDepthTexture->SetMinMagFilter(GL_LINEAR);
		prepassDepthTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassAlbedoTexture = new GLTexture2D();
		prepassAlbedoTexture->Load(GL_RGB32F, nullptr, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT);
		prepassAlbedoTexture->SetMinMagFilter(GL_LINEAR);
		prepassAlbedoTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassViewSpacePositionTexture = new GLTexture2D();
		prepassViewSpacePositionTexture->Load(GL_RGB32F, nullptr, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT);
		prepassViewSpacePositionTexture->SetMinMagFilter(GL_LINEAR);
		prepassViewSpacePositionTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		prepassViewSpaceNormalTexture = new GLTexture2D();
		prepassViewSpaceNormalTexture->Load(GL_RGB32F, nullptr, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT);
		prepassViewSpaceNormalTexture->SetMinMagFilter(GL_LINEAR);
		prepassViewSpaceNormalTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		tileMinMaxTexture = new GLTexture2D();
		tileMinMaxTexture->Load(GL_RG32F, nullptr, WINDOW_WIDTH / TILE_SIZE, WINDOW_HEIGHT / TILE_SIZE, GL_RG, GL_FLOAT);
		tileMinMaxTexture->SetMinMagFilter(GL_LINEAR);
		tileMinMaxTexture->SetWrapST(GL_CLAMP_TO_EDGE);

		/// Depth buffer
		GLCall(glGenRenderbuffers(1, &prepassDepthbuffer));
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, prepassDepthbuffer));
		GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT));

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

		depthShader = new GLShader();
		depthShader->AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/depth_vs.glsl");
		depthShader->AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/depth_fs.glsl");
		depthShader->CompileShaders();

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

#ifdef USE_IMGUI
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
#endif

			for (int x = 0; x < 7; x++)
			{
				for (int z = 0; z < 7; z++)
				{
					lights[x][z].color = vec4(lightIntensity, lightIntensity, lightIntensity, 1.0f);
					vec4 position = vec4(lightGridScale.x * (x - 3) + lightGridOffset.x, lightGridOffset.y, lightGridScale.y * (z - 3) + lightGridOffset.z, 1.0f);
					lights[x][z].viewSpacePosition = V * position;
				}
			}
			lightSSBO->SetData(lights, sizeof(lights));

			switch (renderMode)
			{
			case NONE:
				GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
				GLCall(glClearColor(0.2f, 0.3f, 0.8f, 1.0f));
				GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
				break;

			case DEPTH_ONLY:
				RenderDepthOnly(cubeGrid);
				break;

			case NON_TILED_DEFERRED:
				RenderNonTiledDeferred(cubeGrid);
				break;
			}
			
#ifdef USE_IMGUI
			ImGuiRender();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
			
			glfwSwapBuffers(window);
		}
	}

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

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "TiledShading", NULL, NULL);
	if (!window)
	{
		std::printf("Failed to create glfwWindow!\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

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

	return 0;
}


void ImGuiRender()
{
	ImGui::Begin("Config");

	ImGui::Text("Render mode");
	ImGui::RadioButton("None", (int*)&renderMode, (int)NONE);
	ImGui::RadioButton("Depth only", (int*)&renderMode, (int)DEPTH_ONLY);
	ImGui::RadioButton("Non-tiled, deferred", (int*)&renderMode, (int)NON_TILED_DEFERRED);

	ImGui::Separator();

	ImGui::Text("Light");
	ImGui::SliderFloat("Light intensity", &lightIntensity, 0.0f, 100.0f, "%.1f", 3.0f);
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


void RenderDepthOnly(Cube(&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE])
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *depthShader);

	mat4 V = renderer.camera.GetViewMatrix();
	mat4 P = renderer.camera.projectionMatrix;
	depthShader->SetUniformMat4("u_MV", V);
	depthShader->SetUniformMat4("u_MVP", P * V);
	planeVAO->Bind();
	planeIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, planeIBO->Count(), GL_UNSIGNED_INT, 0));
}

void RenderNonTiledDeferred(Cube(&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE])
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *prepassShader);

	mat4 V = renderer.camera.GetViewMatrix();
	mat4 P = renderer.camera.projectionMatrix;
	prepassShader->SetUniformMat4("u_MV", V);
	prepassShader->SetUniformMat4("u_MV_normal", mat4::Transpose(mat4::Inverse(V)));
	prepassShader->SetUniformMat4("u_MVP", P * V);
	planeVAO->Bind();
	planeIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, planeIBO->Count(), GL_UNSIGNED_INT, 0));

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
	lightSSBO->Bind();
	GLCall(glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, lightSSBO->RendererID(), 0, lightSSBO->Size()));
	fullscreenVAO->Bind();
	fullscreenIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, fullscreenIBO->Count(), GL_UNSIGNED_INT, 0));
}

void RenderTiledDeferred(Cube(&cubeGrid)[CUBE_GRID_SIZE][CUBE_GRID_SIZE])
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, prepassFramebuffer));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (int x = 0; x < 9; x++)
		for (int z = 0; z < 9; z++)
			cubeGrid[x][z].Render(renderer, *prepassShader);

	mat4 V = renderer.camera.GetViewMatrix();
	mat4 P = renderer.camera.projectionMatrix;
	prepassShader->SetUniformMat4("u_MV", V);
	prepassShader->SetUniformMat4("u_MV_normal", mat4::Transpose(mat4::Inverse(V)));
	prepassShader->SetUniformMat4("u_MVP", P * V);
	planeVAO->Bind();
	planeIBO->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, planeIBO->Count(), GL_UNSIGNED_INT, 0));
}