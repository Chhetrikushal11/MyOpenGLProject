#include "triangle_mesh.h"

TriangleMesh::TriangleMesh()
{
    // Vertex positions (4 vertices for a quad)
    std::vector<float> positions = {
        -1.0f, -1.0f, 0.0f,  // 0: bottom left
         1.0f, -1.0f, 0.0f,  // 1: bottom right
        -1.0f,  1.0f, 0.0f,  // 2: top left
         1.0f,  1.0f, 0.0f   // 3: top right
    };

    // Color indices (which color for each vertex)
    std::vector<int> colorIndices = { 0, 1, 2, 3 };

    // Element indices (which vertices form triangles)
    // Triangle 1: vertices 0, 1, 2
    // Triangle 2: vertices 2, 1, 3
    std::vector<unsigned int> elementIndices = { 0, 1, 2,  2, 1, 3 };

    vertex_count = 6;  // Drawing 6 indices (2 triangles)

    // Create VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create 2 VBOs
    VBOs.resize(2);
    glGenBuffers(2, VBOs.data());

    // ========== VBO 0: Positions ==========
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float),
        positions.data(), GL_STATIC_DRAW);

    // Attribute 0: Position (3 floats: x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);
    glEnableVertexAttribArray(0);

    // ========== VBO 1: Color Indices ==========
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, colorIndices.size() * sizeof(int),
        colorIndices.data(), GL_STATIC_DRAW);

    // Attribute 1: Color Index (1 int)
    glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), (void*)0);
    glEnableVertexAttribArray(1);

    // ========== EBO: Element Indices ==========
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementIndices.size() * sizeof(unsigned int),
        elementIndices.data(), GL_STATIC_DRAW);
}

void TriangleMesh::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);
    //                                          ^^^^^^^^^^^^^^^^
    //                                          Fixed typo!
}

TriangleMesh::~TriangleMesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBOs.data());
    glDeleteBuffers(1, &EBO);
}