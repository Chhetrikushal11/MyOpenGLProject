#pragma once
#include "config.h"
#include "component/transform_component.h"
#include "component/physics_component.h"

class MotionSystem
{
public:
	void update
	(
		std::unordered_map<unsigned int, TransformComponent>& transformComponent,
		std::unordered_map<unsigned int, PhysicsComponent>& physicsComponent,
		float dt
	);
};