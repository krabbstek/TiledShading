#pragma once

#include "Renderer.h"
#include "opengl/GLShader.h"

class Renderable
{
public:
	Renderable() {}
	virtual ~Renderable() {}

	virtual void Render(const Renderer& renderer, GLShader& shader) const = 0;
};