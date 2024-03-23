#include "graphics/opengl/OpenGL.h"
#include "math/math.h"

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

GLFWwindow* window;

GLFramebuffer* defaultFramebuffer;

int main()
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

	{
		GLShader shader;
		shader.AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/simple_vs.glsl");
		shader.AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/simple_fs.glsl");
		shader.CompileShaders();

		GLShader fullscreenShader;
		fullscreenShader.AddShaderFromFile(GL_VERTEX_SHADER, "res/shaders/fullscreen_vs.glsl");
		fullscreenShader.AddShaderFromFile(GL_FRAGMENT_SHADER, "res/shaders/fullscreen_fs.glsl");
		fullscreenShader.CompileShaders();

		float vertices[] =
		{
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
		};
		unsigned int indices[] =
		{
			0, 1, 2,
		};

		GLVertexBuffer vbo(vertices, sizeof(vertices));
		GLVertexBufferLayout layout;
		layout.Push(GL_FLOAT, 3);
		vbo.SetVertexBufferLayout(layout);
		GLVertexArray vao;
		vao.AddVertexBuffer(vbo);
		GLIndexBuffer ibo(indices, sizeof(indices) / sizeof(unsigned int));

		float fullscreenVertices[] =
		{
			-1.0f, -1.0f,  0.5f,  0.0f, 1.0f,
			 1.0f, -1.0f,  0.5f,  1.0f, 1.0f,
			 1.0f,  1.0f,  0.5f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.5f,  0.0f, 0.0f,
		};
		unsigned int fullscreenIndices[] =
		{
			0, 1, 2,
			0, 2, 3,
		};

		GLVertexBuffer fsVBO(fullscreenVertices, sizeof(fullscreenVertices));
		layout.Push(GL_FLOAT, 2);
		fsVBO.SetVertexBufferLayout(layout);
		GLVertexArray fsVAO;
		fsVAO.AddVertexBuffer(fsVBO);
		GLIndexBuffer fsIBO(fullscreenIndices, sizeof(fullscreenIndices) / sizeof(unsigned int));

		defaultFramebuffer = GLFramebuffer::GetDefaultFramebuffer();

		GLFramebuffer intermediateFramebuffer(80, 60);
		GLTexture2D intermediateFramebufferTexture;
		intermediateFramebuffer.AttachTexture(intermediateFramebufferTexture, GL_RGB8, GL_UNSIGNED_BYTE, 0);
		intermediateFramebuffer.Bind();
		//GLCall(glClearColor(0.67f, 0.94f, 0.36f, 0.0f));

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();

			intermediateFramebuffer.Bind();
			GLCall(glClearColor(0.67f, 0.94f, 0.36f, 0.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			shader.Bind();
			vao.Bind();
			ibo.Bind();
			GLCall(glDrawElements(GL_TRIANGLES, ibo.Count(), GL_UNSIGNED_INT, 0));

			defaultFramebuffer->Bind();
			GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

			fullscreenShader.Bind();
			intermediateFramebufferTexture.Bind();
			fsVAO.Bind();
			fsIBO.Bind();
			GLCall(glDrawElements(GL_TRIANGLES, fsIBO.Count(), GL_UNSIGNED_INT, 0));

			glfwSwapBuffers(window);
		}

		delete defaultFramebuffer;
	}

	glfwTerminate();

	return 0;
}