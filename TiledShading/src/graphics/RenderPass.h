#pragma once

#include "Renderable.h"

#include <vector>

struct RenderableArgs
{
	const Renderable* renderable;
	const Renderer* renderer;
	GLShader* shader;

	RenderableArgs(const Renderable* renderable, const Renderer* renderer, GLShader* shader)
		: renderable(renderable), renderer(renderer), shader(shader) {}
};

class RenderPass
{
public:
	RenderPass() {}
	virtual ~RenderPass() {}

	virtual void Render(std::vector<RenderableArgs>& renderables);
};