#include "config.h"
#include "app/app.h"

#include "component/camera_component.h"
#include "component/physics_component.h"
#include "component/render_component.h"
#include "component/transform_component.h"

int main()
{

	App* app = new App();

	unsigned int cubeEntity = app->make_entity();
	TransformComponent transform;
	transform.position = { 3.0f, 0.0f,  0.25f };
	transform.eulers = { 0.0f, 0.0f, 0 - .0f };
	app->transformComponents[cubeEntity] = transform;

	PhysicsComponent physics;
	physics.velocity = { 0.0f, 0.0f, 0.0f };
	physics.eulerVelocity = { 0.0f, 0.0f, 0.0f };
	app->physicsComponents[cubeEntity] = physics;

	RenderComponent render;
	render.mesh = app->make_cube_mesh({ 0.25f, 0.25f, 0.25f });
	render.material = app->make_texture("../img/paper.jpg");
	app->renderComponents[cubeEntity] = render;

	unsigned int cameraEntity = app->make_entity();
	transform.position = { 0.0f, 0.0f, 1.0f };
	transform.eulers = { 0.0f, 0.0f, 0.0f };
	app->transformComponents[cubeEntity] = transform;

	CameraComponent* camera = new CameraComponent();
	app->cameraComponent = camera;
	app->cameraID = cameraEntity;

	app->set_up_opengl();
	app->make_systems();

	app->run();

	delete app;
	return 0;

}