#include "Camera.h"

mat4 Camera::GetViewMatrix() const
{
	return mat4::RotateZ(-rotation.z) * mat4::RotateX(-rotation.x) * mat4::RotateY(-rotation.y) * mat4::Translate(-position);
}