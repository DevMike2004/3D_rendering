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
#include <glm/gtx/string_cast.hpp>


const int WIN_WIDTH = 800, WIN_HEIGHT = 600;
const float FOV = 45.0;

// --- camera state -----------------------------------------------------------

Camera cam = Camera(FOV, WIN_WIDTH, WIN_HEIGHT);

std::vector<float> generateCircle(float cx, float cy, float radius, int segments);

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

    // if on mac, use this preprocessing method for mac os compatibility.
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

    const unsigned int segments = 64;
    // unit circle at the origin -- move/scale it with the model matrix

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
    Entity sphere, sphere2, mesh;

    std::vector<float> sphereVerts = sphere.generateSphere(std::vector<float> {0.0f, 0.0f, 0.0f},
            1.0f, segments);
    std::vector<float> sphereVerts2 = sphere2.generateSphere(std::vector<float> {2.0f, 0.0f, 2.0f},
            1.0f, segments);
    std::vector<float> meshVerts = mesh.makeGrid(30.0f, 1.0f);

    sphere.genBufferObject(sphereVerts, GL_STATIC_DRAW, true, sphereIndices);
    sphere2.genBufferObject(sphereVerts2, GL_STATIC_DRAW, true, sphereIndices);
    mesh.genBufferObject(meshVerts, GL_STATIC_DRAW, false);

    const GLsizei vertexCount = (GLsizei)(sphereVerts.size() / 3);

    // the actual window process
    while (!glfwWindowShouldClose(window)) {

        float orbitRadius = 3.0f;
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
        model = glm::scale(model, glm::vec3(0.5f));   // radius 0.5
        

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        float sphereX = (float)cosf(currentFrame) * orbitRadius;
        float sphereZ = (float)sinf(currentFrame) * orbitRadius;
        glm::vec3 sphereTranslate = glm::vec3(sphereX, 0.0f, sphereZ);

        std::vector<float> pos = sphere2.getPos(sphere.position, sphereTranslate);
        auto point = pos.begin();

        while (point != pos.end() + 1){
            if (point == pos.begin()) std::cout << '[';

            point == pos.end() ? std::cout << ']' << std::endl : std::cout << *point << ", ";
            ++point;
        }


        model = glm::translate(model, sphereTranslate);
        ourShader.setMat4("model", model);
        glBindVertexArray(sphere2.VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);

        //glDrawArrays(GL_POINTS, 0, vertexCount);

        ourShader.setVec4("color", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
        
        model = glm::translate(model, glm::vec3(-3.0f));
        model = glm::scale(model, glm::vec3(1.0f));
        ourShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(mesh.VAO);
        glDrawArrays(GL_LINES, 0, meshVerts.size() / 3);

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

#include <vector>

// Build grid lines on the XZ plane, centered at the origin.
// cellSize stays constant; cellsPerSide controls overall size.
