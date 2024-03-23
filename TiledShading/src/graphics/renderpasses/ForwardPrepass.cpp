#include "ForwardPrepass.h"

#include "Globals.h"
#include "graphics/opengl/OpenGL.h"

void ForwardPrepass::Render(std::vector<RenderableArgs>& renderables)
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glViewport(0, 0, g_WindowWidth, g_WindowHeight));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	for (RenderableArgs renderableArgs : renderables)
		renderableArgs.renderable->Render(*renderableArgs.renderer, *m_Shader);
}