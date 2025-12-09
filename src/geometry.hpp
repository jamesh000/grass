#include <glad/glad.h>

struct Mesh {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    GLsizei indexCount;
};


Mesh generateGrassBillboard();
Mesh generateFlatPlane();
