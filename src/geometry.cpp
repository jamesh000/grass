#include "geometry.hpp"

Mesh generateGrassBillboard()
{
    Mesh m{};
/*    float vertices[] = {
        // pos.x, pos.y, pos.z,  tex.u, tex.v
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f
        };*/
    float vertices[] = {
        // pos.x, pos.y, pos.z,  tex.u, tex.v
         0.5f,  1.0f, 0.0f,   1.0f, 1.0f,
         0.5f, 0.0f, 0.0f,   1.0f, 0.0f,
        -0.5f, 0.0f, 0.0f,   0.0f, 0.0f,
        -0.5f,  1.0f, 0.0f,   0.0f, 1.0f
        };
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.EBO);

    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texcoord (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Keep EBO bound to VAO. Unbind VAO only.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m.indexCount = 6;
    return m;
}

Mesh generateFlatPlane()
{
    Mesh m{};
    float vertices[] = {
        // pos.x, pos.y, pos.z,   col.r, col.g, col.b
         0.5f,  0.0f,  0.5f,     0.45f, 0.40f, 0.23f,
         0.5f,  0.0f, -0.5f,     0.45f, 0.40f, 0.23f,
        -0.5f,  0.0f, -0.5f,     0.45f, 0.40f, 0.23f,
        -0.5f,  0.0f,  0.5f,     0.45f, 0.40f, 0.23f
    };
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.EBO);

    glBindVertexArray(m.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m.indexCount = 6;
    return m;
}
