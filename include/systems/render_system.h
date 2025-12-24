#pragma once
#include "config.h"
#include "component/transform_component.h"
#include "component/render_component.h"

class RenderSystem
{
public:
	RenderSystem(unsigned int shader, GLFWwindow* window);

	void update
	(
		std::unordered_map<unsigned int, TransformComponent>& transformComponent,
		std::unordered_map<unsigned int, RenderComponent>& renderComponent
	);

private:
	unsigned int shader;
	unsigned int modelLocation;
	GLFWwindow* window;
};
