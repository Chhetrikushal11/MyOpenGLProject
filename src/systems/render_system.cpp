#include "systems/render_system.h"

RenderSystem::RenderSystem(unsigned int shader, GLFWwindow* window)
{
    this->shader = shader;  // Store the shader program ID
    modelLocation = glGetUniformLocation(shader, "model");
    this->window = window;
    
    std::cout << "RenderSystem initialized" << std::endl;
    std::cout << "  Shader program: " << shader << std::endl;
    std::cout << "  Model uniform location: " << modelLocation << std::endl;
}

void RenderSystem::update(
    std::unordered_map<unsigned int, TransformComponent>& transformComponent, 
    std::unordered_map<unsigned int, RenderComponent>& renderComponent)
{
    // CRITICAL: Make sure the shader is active!
    glUseProgram(shader);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static bool firstFrame = true;
    if (firstFrame) {
        std::cout << "=== RENDER SYSTEM DEBUG ===" << std::endl;
        std::cout << "Number of entities to render: " << renderComponent.size() << std::endl;
        firstFrame = false;
    }
    
    for (std::pair<unsigned int, RenderComponent> entity : renderComponent)
    {
        static int frameCount = 0;
        if (frameCount < 5) {
            std::cout << "Rendering entity " << entity.first << std::endl;
            std::cout << "  VAO: " << entity.second.mesh << std::endl;
            std::cout << "  Texture: " << entity.second.material << std::endl;
            std::cout << "  Position: " << transformComponent[entity.first].position.x 
                      << ", " << transformComponent[entity.first].position.y 
                      << ", " << transformComponent[entity.first].position.z << std::endl;
        }
        
        TransformComponent& transform = transformComponent[entity.first];
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        model = glm::rotate(
            model, glm::radians(transform.eulers.z),
            { 0.0f, 0.0f, 1.0f });
        
        glUniformMatrix4fv(
            modelLocation, 1, GL_FALSE,
            glm::value_ptr(model)
        );
        
        // Bind texture to texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, entity.second.material);
        glBindVertexArray(entity.second.mesh);
        
        // Changed from glDrawArrays to glDrawElements
        // 36 indices for a cube (6 faces * 2 triangles * 3 vertices)
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        
        // Check for OpenGL errors
        GLenum err = glGetError();
        if (err != GL_NO_ERROR && frameCount < 5) {
            std::cout << "  OpenGL Error: " << err << std::endl;
        }
        
        frameCount++;
    }
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}