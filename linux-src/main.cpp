#include <cmath>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "Camera.h"
#include "Entity.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int WIN_WIDTH = 800, WIN_HEIGHT = 600;
const float FOV = 45.0;

// --- camera state -----------------------------------------------------------

Camera cam = Camera(FOV, WIN_WIDTH, WIN_HEIGHT);

std::vector<float> generateCircle(float cx, float cy, float radius, int segments);
std::vector<float> generateSphere(float cx, float cy, float cz, float radius, int segments);

// the functions needed (defined at bottom of file)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void glfwErrorCallback(int code, const char* desc);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {

    glfwSetErrorCallback(glfwErrorCallback);
    // initializing glfw
    glfwInit();
    // setting the versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // need this for all operating systems
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // if on mac, use this preprocessing method for mac os compatability.
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "grav-proj");

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwSetWindowUserPointer(window, &cam);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);



    // hide + lock the cursor so the trackpad drives the look direction
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);



    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }


    glEnable(GL_DEPTH_TEST);

    // Loading the shader files

    // Mac dir tree path
    Shader ourShader("shaders/vertexShader.vert",
                     "shaders/fragmentShader.frag");

    unsigned int segments = 64;
    // unit circle at the origin -- move/scale it with the model matrix
    std::vector<float> sphereVerts = generateSphere(0.0f, 0.0f, 0.0f, 1.0f, segments);
    std::vector<unsigned int> sphereIndices;

    int stacks   = segments / 2;     // rings pole-to-pole
    int sectors  = segments;         // points around each ring
    int stride   = segments + 1;     // points PER ring (the closing-duplicate column)

    for (int i = 0; i < stacks; ++i) {          // note: < , stop one short
        for (int j = 0; j < sectors; ++j) {     // each patch reaches to i+1, j+1

            unsigned int topLeft     = i * stride + j;
            unsigned int topRight    = i * stride + (j + 1);
            unsigned int bottomLeft  = (i + 1) * stride + j;
            unsigned int bottomRight = (i + 1) * stride + (j + 1);

            // triangle 1
            sphereIndices.push_back(topLeft);
            sphereIndices.push_back(bottomLeft);
            sphereIndices.push_back(topRight);

            // triangle 2
            sphereIndices.push_back(topRight);
            sphereIndices.push_back(bottomLeft);
            sphereIndices.push_back(bottomRight);
        }
    }

    // vertex buffer and array objects
    Entity sphere;
    sphere.genElementBufferObject(sphereVerts, sphereIndices, GL_STATIC_DRAW);

    const GLsizei vertexCount = (GLsizei)(sphereVerts.size() / 3);

    // the actual window process
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = (float)glfwGetTime();
        cam.deltaTime = currentFrame - cam.lastFrame;
        cam.lastFrame = currentFrame;

        cam.processInput(window);

        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 view = glm::lookAt(cam.cameraPos, cam.cameraPos + cam.cameraFront, cam.cameraUp);

        glm::mat4 projection = glm::perspective(glm::radians(cam.fov),
                                                (float)WIN_WIDTH / (float)WIN_HEIGHT,
                                                0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));   // radius 0.5

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        glBindVertexArray(sphere.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &sphere.VAO);
    glDeleteBuffers(1, &sphere.VBO);
    glDeleteBuffers(1, &sphere.EBO);

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// This generates a circle around a center point using TRIANGLES_FAN
// later in the glDrawArrays function
//std::vector<float> generateCircle(float cx, float cy, float radius, int segments) {
//    std::vector<float> verts;
//    verts.reserve((segments + 2) * 3);
//
//    verts.push_back(cx);
//    verts.push_back(cy);
//    verts.push_back(0.0f);
//
//    for (int i = 0; i <= segments; ++i) {
//        float angle = 2.0f * (float)M_PI * i / segments;
//        verts.push_back(cx + radius * cosf(angle));
//        verts.push_back(cy + radius * sinf(angle));
//        verts.push_back(0.0f);
//    }
//
//    return verts;
//}

std::vector<float> generateSphere(float cx, float cy, float cz, float radius, int segments) {
    
    std::vector<float> verts;
    verts.reserve((segments + 1) * (segments /2 + 1) * 3);

    for (int i = 0; i <= segments/2; ++i){
        float stackAngle = (float)M_PI * i / (segments / 2.0f);
        float ringRadius = radius * sin(stackAngle);
        float height = radius * cos(stackAngle);

        for (int j = 0; j <= segments; ++j) {
            float angle = 2.0f * (float)M_PI * j / segments;
            verts.push_back(ringRadius * cosf(angle));
            verts.push_back(ringRadius * sinf(angle));
            verts.push_back(height);
        }
    }

    return verts;
}

static void glfwErrorCallback(int code, const char* desc) {
    std::cerr << "GLFW error " << code << ": " << desc << std::endl;
}

// reference the object via pointer passing in the window so that the c code
// doesn't realize it's a class object
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
    cam->processScroll((float)xoffset, (float)yoffset);
}

// same here
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    Camera* cam = (Camera*)glfwGetWindowUserPointer(window);
    cam->processMouse((float)xpos, (float)ypos);
}
