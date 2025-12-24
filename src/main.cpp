#include "config.h"
#include "app/app.h"
#include "component/camera_component.h"
#include "component/physics_component.h"
#include "component/render_component.h"
#include "component/transform_component.h"

int main() {
    App* app = new App();
    
    app->set_up_opengl();
    app->make_systems();
    
    // Create the Cube - PUT IT IN FRONT OF CAMERA
    unsigned int cubeEntity = app->make_entity();
    TransformComponent cubeTransform;
    cubeTransform.position = {0.0f, -3.0f, 0.0f}; // Camera looks at -Y initially
    cubeTransform.eulers = {0.0f, 0.0f, 0.0f};
    app->transformComponents[cubeEntity] = cubeTransform;
    
    PhysicsComponent physics;
    physics.velocity = {0.0f, 0.0f, 0.0f};
    physics.eulerVelocity = {45.0f, 45.0f, 45.0f}; // Rotate on all axes for visibility
    app->physicsComponents[cubeEntity] = physics;
    
    RenderComponent render;
    render.mesh = app->make_cube_mesh({0.5f, 0.5f, 0.5f});
    render.material = app->make_texture("img/paper.jpg"); 
    app->renderComponents[cubeEntity] = render;
    
    // Create the Camera
    unsigned int cameraEntity = app->make_entity();
    TransformComponent camTransform;
    camTransform.position = {0.0f, 0.0f, 0.0f}; // Camera at origin
    camTransform.eulers = {0.0f, 0.0f, 270.0f}; // Look down -Y axis
    app->transformComponents[cameraEntity] = camTransform;
    
    CameraComponent* camera = new CameraComponent();
    app->cameraComponent = camera;
    app->cameraID = cameraEntity;
    
    std::cout << "Camera at: " << camTransform.position.x << ", " 
              << camTransform.position.y << ", " << camTransform.position.z << std::endl;
    std::cout << "Cube at: " << cubeTransform.position.x << ", " 
              << cubeTransform.position.y << ", " << cubeTransform.position.z << std::endl;
    std::cout << "Starting render loop..." << std::endl;
    
    app->run();
    delete app;
    return 0;
}