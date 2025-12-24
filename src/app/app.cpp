#include "app/app.h"
#include "config.h"
#include "stb_image.h"
#include <vector>

App::App() {
    set_up_glfw();
}

App::~App() {
    glDeleteProgram(shader);

    delete motionSystem;
    delete cameraSystem;
    delete renderSystem;
    
    glfwTerminate();
}

void App::run() {
    while (!glfwWindowShouldClose(window)) {
        motionSystem->update(
            transformComponents, physicsComponents, 16.67f/1000.0f);
        bool should_close = cameraSystem->update(
            transformComponents, cameraID, *cameraComponent, 16.67f/1000.0f);
        if (should_close) {
            break;
        }

        renderSystem->update(transformComponents, renderComponents);
    }
}

void App::set_up_glfw() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    
    window = glfwCreateWindow(640, 480, "Hello Window!", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Couldn't load opengl" << std::endl;
        glfwTerminate();
    }
}

void App::set_up_opengl() {
    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    // Set the rendering region to the actual screen size
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    // (left, top, width, height)
    glViewport(0, 0, w, h);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Temporarily disable face culling to debug
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    shader = make_shader(
        "shaders/vertex.txt", 
        "shaders/fragment.txt");
    
    glUseProgram(shader);
    
    // Set texture unit for material sampler
    unsigned int materialLocation = glGetUniformLocation(shader, "material");
    glUniform1i(materialLocation, 0); // Use texture unit 0
    
    unsigned int projLocation = glGetUniformLocation(shader, "projection");
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 10.0f);
    glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection));
    
    std::cout << "OpenGL setup complete" << std::endl;
    std::cout << "Shader program: " << shader << std::endl;
    std::cout << "Projection location: " << projLocation << std::endl;
    std::cout << "Material location: " << materialLocation << std::endl;
}

void App::make_systems() {
    motionSystem = new MotionSystem();
    cameraSystem = new CameraSystem(shader, window);
    renderSystem = new RenderSystem(shader, window);
}

unsigned int App::make_entity() {
    static unsigned int entityCounter = 0;
    return entityCounter++;
}

unsigned int App::make_cube_mesh(glm::vec3 size) {
    std::cout << "Creating cube mesh with size: " << size.x << ", " << size.y << ", " << size.z << std::endl;
    
    // Cube vertices with positions and texture coordinates
    std::vector<float> vertices = {
        // Front face
        -size.x, -size.y,  size.z,  0.0f, 0.0f,
         size.x, -size.y,  size.z,  1.0f, 0.0f,
         size.x,  size.y,  size.z,  1.0f, 1.0f,
        -size.x,  size.y,  size.z,  0.0f, 1.0f,
        
        // Back face
        -size.x, -size.y, -size.z,  1.0f, 0.0f,
        -size.x,  size.y, -size.z,  1.0f, 1.0f,
         size.x,  size.y, -size.z,  0.0f, 1.0f,
         size.x, -size.y, -size.z,  0.0f, 0.0f,
        
        // Top face
        -size.x,  size.y, -size.z,  0.0f, 1.0f,
        -size.x,  size.y,  size.z,  0.0f, 0.0f,
         size.x,  size.y,  size.z,  1.0f, 0.0f,
         size.x,  size.y, -size.z,  1.0f, 1.0f,
        
        // Bottom face
        -size.x, -size.y, -size.z,  0.0f, 0.0f,
         size.x, -size.y, -size.z,  1.0f, 0.0f,
         size.x, -size.y,  size.z,  1.0f, 1.0f,
        -size.x, -size.y,  size.z,  0.0f, 1.0f,
        
        // Right face
         size.x, -size.y, -size.z,  1.0f, 0.0f,
         size.x,  size.y, -size.z,  1.0f, 1.0f,
         size.x,  size.y,  size.z,  0.0f, 1.0f,
         size.x, -size.y,  size.z,  0.0f, 0.0f,
        
        // Left face
        -size.x, -size.y, -size.z,  0.0f, 0.0f,
        -size.x, -size.y,  size.z,  1.0f, 0.0f,
        -size.x,  size.y,  size.z,  1.0f, 1.0f,
        -size.x,  size.y, -size.z,  0.0f, 1.0f
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,       // Front
        4, 5, 6, 6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,    // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 
                 vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
                         (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    std::cout << "Cube mesh created with VAO: " << VAO << std::endl;
    
    return VAO;
}

unsigned int App::make_texture(const char* filename) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load image
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    
    if (data) {
        std::cout << "Texture loaded successfully: " << filename << std::endl;
        std::cout << "  Dimensions: " << width << "x" << height << std::endl;
        std::cout << "  Channels: " << nrChannels << std::endl;
        
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, 
                     format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture: " << filename << std::endl;
        std::cout << "  STB Error: " << stbi_failure_reason() << std::endl;
        
        // Create a simple white texture as fallback
        unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
    }
    
    stbi_image_free(data);
    return texture;
}