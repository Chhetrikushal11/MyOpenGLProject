#include "systems/motion_system.h"

void MotionSystem::update(std::unordered_map<unsigned int, TransformComponent>& transformComponent, std::unordered_map<unsigned int, PhysicsComponent>& physicsComponent, float dt)
{
	for (std::pair<unsigned int, PhysicsComponent> entity : physicsComponent)
	{
		transformComponent[entity.first].position += entity.second.velocity * dt;
		transformComponent[entity.first].eulers += entity.second.eulerVelocity * dt;
		if (transformComponent[entity.first].eulers.z > 360)
		{
			transformComponent[entity.first].eulers.z -= 360;
		}

	}
}
