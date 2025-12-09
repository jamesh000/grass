#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "shader.h"
#include "camera.h"

#include "geometry.hpp"

// Rendering parameters
const unsigned int planeSize = 150; // side length of the square plane
const unsigned int grassDensity = 100; // grass per unit squared

// screen width and height
int screenWidth = 800, screenHeight = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = (float)screenWidth / 2;
float lastY = (float)screenHeight / 2;
bool firstMouse = true;

// timing
float deltaT = 0.0f;
float oldFrameTime = 0.0f;

void fbSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

// From LearnOpenGL.com
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaT);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaT);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaT);
}

// From LearnOpenGL.com
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

int main()
{
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Anti aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);        

    GLFWwindow* window = glfwCreateWindow(800, 600, "Window1", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, fbSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);

    // capture mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Disable vsync
    glfwSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);
    
    // enable depth test
    glEnable(GL_DEPTH_TEST);

    // enable anti aliasing
    glEnable(GL_MULTISAMPLE);

    Shader grassShader("../shaders/grass.vs", "../shaders/grass.fs");
    Shader planeShader("../shaders/plane.vs", "../shaders/plane.fs");

    // Grass geometry
    Mesh grassMesh = generateGrassBillboard();
    Mesh planeMesh = generateFlatPlane();

    // load texture
    unsigned int grassTexture;

    glGenTextures(1, &grassTexture);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    // Texture wrapping params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // texture filtering params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, channelNo;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *image = stbi_load("../grass.png", &width, &height, &channelNo, 0);
    if (!image) {
        std::cout << "Failed to load texture" << std::endl;
        glfwTerminate();
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(image);

    // transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set shader uniform
    grassShader.use();
    grassShader.setInt("grasstex", 0);


    // Generate model tranform array for grass
    unsigned int grassAmount = planeSize * planeSize * grassDensity;
    glm::mat4* grassMatrices;
    grassMatrices = new glm::mat4[grassAmount];
    srand(static_cast<unsigned int>(glfwGetTime()));
    float planeMax = (float)planeSize/4.0f;
    unsigned int grassCount = 0;
    for (float i = -planeMax; i < planeMax; i += 1.0f) {
        for (float j = -planeMax; j < planeMax; j += 1.0f) {
            for (int k = 0; k < grassDensity; k++) {
                glm::mat4 grassModel = glm::mat4(1.0f);


                // Translation
                float xOffset = (float)rand()/(float)RAND_MAX;
                float zOffset = (float)rand()/(float)RAND_MAX;
                grassModel = glm::translate(grassModel, glm::vec3(i + xOffset, 0.0f, j + zOffset));

                // Scale
                float scaleOffset = (float)rand()/((float)RAND_MAX * 2);
                float scale = 0.5f + scaleOffset;
                grassModel = glm::scale(grassModel, glm::vec3(scale, scale, scale));

                // Rotate
                float rotY = ((float)rand()/RAND_MAX) * 360.0f;
                float rotX = (((float)rand()/RAND_MAX) - 0.5f) * 10.0f; 
                grassModel = glm::rotate(grassModel, glm::radians(rotY), glm::vec3(0,1,0));
                grassModel = glm::rotate(grassModel, glm::radians(rotX), glm::vec3(1,0,0));


                if (grassCount >= grassAmount)
                    break;
                grassMatrices[grassCount] = grassModel;
                grassCount++;
            }
        }
    }

    // Create array
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, grassAmount * sizeof(glm::mat4), &grassMatrices[0], GL_STATIC_DRAW);

    glBindVertexArray(grassMesh.VAO);
    // set attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
    
    // for fps counter
    float secondAgo = glfwGetTime();
    int frameCount = 0;
    while (!glfwWindowShouldClose(window)) {
        // Frame timing
        float frameTime = static_cast<float>(glfwGetTime());
        deltaT = frameTime - oldFrameTime;
        oldFrameTime = frameTime;

        frameCount++;
        if (frameTime - secondAgo >= 1.0f ) {
            std::cout << "fps: " << frameCount << " / ms/frame: " << 1000.0/double(frameCount) << "\n";
            frameCount = 0;
            secondAgo = frameTime;
        }

        // Input
        processInput(window);
        
        // BG Color
        glClearColor(0.53f, 0.80f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // transformation matrices
        glm::mat4 p = glm::perspective(glm::radians(90.0f), (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
        //glm::mat4 p = glm::mat4(1.0f);
        glm::mat4 v = camera.GetViewMatrix();
        //std::cout << glm::to_string(p) << std::endl;
        grassShader.use();
        grassShader.setMat4("p", p);
        grassShader.setMat4("v", v);
        planeShader.use();
        planeShader.setMat4("p", p);
        planeShader.setMat4("v", v);

        // Draw plane
        float planeScale = (float)planeSize/2.0f;
        glm::mat4 planeModel = glm::mat4(1.0f);
        //planeModel = glm::translate(planeModel, glm::vec3(0.0f, 0.0f, 0.0f));
        planeModel = glm::scale(planeModel, glm::vec3(planeScale, planeScale, planeScale));
        planeShader.setMat4("m", planeModel);
        glBindVertexArray(planeMesh.VAO);
        glDrawElements(GL_TRIANGLES, planeMesh.indexCount, GL_UNSIGNED_INT, 0);

        // Draw grass
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        grassShader.use();
        glBindVertexArray(grassMesh.VAO);
        //glDrawElements(GL_TRIANGLES, grassMesh.indexCount, GL_UNSIGNED_INT, 0);
        glDrawElementsInstanced(GL_TRIANGLES, grassMesh.indexCount, GL_UNSIGNED_INT, 0, grassAmount);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
