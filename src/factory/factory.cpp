#include "factory/factory.h"
#include <stb_image.h>

Factory::Factory(
    std::unordered_map<unsigned int, PhysicsComponent>& physicsComponents,
    std::unordered_map<unsigned int, RenderComponent>& renderComponents,
    std::unordered_map<unsigned int, TransformComponent>& transformComponents) :
    physicsComponents(physicsComponents),
    renderComponents(renderComponents),
    transformComponents(transformComponents) {
}

Factory::~Factory() {
    glDeleteBuffers(VBOs.size(), VBOs.data());
    glDeleteVertexArrays(VAOs.size(), VAOs.data());
    glDeleteTextures(textures.size(), textures.data());
}

unsigned int Factory::make_camera(glm::vec3 position, glm::vec3 eulers) {

    TransformComponent transform;
    transform.position = position;
    transform.eulers = eulers;

    transformComponents[entities_made] = transform;

    return entities_made++;
}

void Factory::make_cube(glm::vec3 position, glm::vec3 eulers,
    glm::vec3 eulerVelocity) {

    TransformComponent transform;
    transform.position = position;
    transform.eulers = eulers;
    transformComponents[entities_made] = transform;

    PhysicsComponent physics;
    physics.velocity = { 0.0f, 0.0f, 0.0f };
    physics.eulerVelocity = eulerVelocity;
    physicsComponents[entities_made] = physics;

    RenderComponent render = make_cube_mesh({ 0.25f, 0.25f, 0.25f });
    render.material = make_texture("../img/paper.jpg");
    renderComponents[entities_made++] = render;
}

void Factory::make_girl(glm::vec3 position, glm::vec3 eulers) {
    TransformComponent transform;
    transform.position = position;
    transform.eulers = eulers;
    transformComponents[entities_made] = transform;

    // FIXED: Now properly stores the render component
    RenderComponent render = make_obj_mesh("../models/girl.obj");
    render.material = make_texture("../img/stargirl.png");
    renderComponents[entities_made++] = render;
}

RenderComponent Factory::make_obj_mesh(const char* filepath) {
    // Temporary storage arrays
    std::vector<glm::vec3> v;
    std::vector<glm::vec2> vt;
    std::vector<glm::vec3> vn;

    // FIXED: Declare vertices vector
    std::vector<float> vertices;

    size_t vertexCount = 0;
    size_t texcoordCount = 0;
    size_t normalCount = 0;

    std::string line;
    std::vector<std::string> words;

    std::ifstream file;

    // First pass: count elements
    file.open(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open model file: " << filepath << std::endl;
        return RenderComponent();  // Return empty component
    }

    while (std::getline(file, line)) {
        words = split(line, " ");

        if (!words[0].compare("v")) {
            ++vertexCount;
        }
        else if (!words[0].compare("vt")) {
            ++texcoordCount;
        }
        else if (!words[0].compare("vn")) {
            ++normalCount;
        }
    }
    file.close();

    // Reserve space
    v.reserve(vertexCount);
    vt.reserve(texcoordCount);
    vn.reserve(normalCount);

    // Second pass: read data
    file.open(filepath);
    while (std::getline(file, line)) {
        words = split(line, " ");

        if (!words[0].compare("v")) {
            v.push_back(read_vec3(words));
        }
        else if (!words[0].compare("vt")) {
            vt.push_back(read_vec2(words));
        }
        else if (!words[0].compare("vn")) {
            vn.push_back(read_vec3(words));
        }
        else if (!words[0].compare("f")) {
            read_face(words, v, vt, vn, vertices);
        }
    }
    file.close();

    std::cout << "Loaded model: " << filepath << std::endl;
    std::cout << "\tVertices: " << v.size() << std::endl;
    std::cout << "\tTex coords: " << vt.size() << std::endl;
    std::cout << "\tNormals: " << vn.size() << std::endl;
    std::cout << "\tFinal vertex data count: " << vertices.size() / 8 << " vertices" << std::endl;

    // FIXED: Create OpenGL buffers
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    VAOs.push_back(VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    VBOs.push_back(VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
        vertices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)12);
    glEnableVertexAttribArray(1);

    // Normal attribute (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)20);
    glEnableVertexAttribArray(2);

    // FIXED: Return RenderComponent
    RenderComponent record;
    record.VAO = VAO;
    record.vertexCount = vertices.size() / 8;  // 8 floats per vertex (3 pos + 2 tex + 3 normal)

    return record;
}

void Factory::read_corner(
    std::string descriptions,
    std::vector<glm::vec3>& v,
    std::vector<glm::vec2>& vt,   // FIXED: Correct type and order
    std::vector<glm::vec3>& vn,   // FIXED: Correct type and order
    std::vector<float>& vertices
) {
    std::vector<std::string> v_vt_vn = split(descriptions, "/");

    // Position
    glm::vec3 pos = v[std::stol(v_vt_vn[0]) - 1];
    vertices.push_back(pos[0]);
    vertices.push_back(pos[1]);
    vertices.push_back(pos[2]);

    // Texture coordinates
    glm::vec2 tex = vt[std::stol(v_vt_vn[1]) - 1];
    vertices.push_back(tex[0]);
    vertices.push_back(tex[1]);

    // Normal
    glm::vec3 normal = vn[std::stol(v_vt_vn[2]) - 1];
    vertices.push_back(normal[0]);
    vertices.push_back(normal[1]);
    vertices.push_back(normal[2]);
}