#include <cmath>
#include <iostream>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const int WIN_WIDTH = 800, WIN_HEIGHT = 600;

// --- camera state -----------------------------------------------------------
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw   = -90.0f;   // -90 so we start looking down -Z, not +X
float pitch = 0.0f;
float fov   = 45.0f;

bool  firstMouse = true;
float lastX = WIN_WIDTH / 2.0f;
float lastY = WIN_HEIGHT / 2.0f;

// --- timing -----------------------------------------------------------------
float deltaTime = 0.0f;   // seconds since last frame
float lastFrame = 0.0f;

// 
std::vector<float> generateCircle(float cx, float cy, float radius, int segments);
std::vector<float> generateSphere(float cx, float cy, float cz, float radius, int segments);

// the functions needed (defined at bottom of file)
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
static void glfwErrorCallback(int code, const char* desc);

int main() {

    glfwSetErrorCallback(glfwErrorCallback);
    // initializing glfw
    glfwInit();
    // setting the versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // need this for all operating systems
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // this is needed only for mac os
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "grav-proj");

    GLFWwindow* window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "Window", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

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
    Shader ourShader("/Users/michael/Code-Projects/C++/grav-proj/mac-src/shaders/vertexShader.vert",
                     "/Users/michael/Code-Projects/C++/grav-proj/mac-src/shaders/fragmentShader.frag");





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
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float),
                 sphereVerts.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int),
            sphereIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    const GLsizei vertexCount = (GLsizei)(sphereVerts.size() / 3);



    // the actual window process
    while (!glfwWindowShouldClose(window)) {

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(.2f, .3f, .3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection = glm::perspective(glm::radians(fov),
                                                (float)WIN_WIDTH / (float)WIN_HEIGHT,
                                                0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f));   // radius 0.5

        ourShader.setMat4("model", model);
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);


        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_POINTS, 0, vertexCount);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}














void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
















// WASD to fly, space/ctrl for up and down, ESC to quit
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // scale by deltaTime so speed doesn't depend on framerate
    const float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cameraPos -= cameraUp * cameraSpeed;
}












// trackpad / mouse look
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    // first event is a huge jump from the default (0,0) -- swallow it
    if (firstMouse) {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;   // reversed: screen y grows downward
    lastX = (float)xpos;
    lastY = (float)ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    // stop the view flipping over at the poles
    if (pitch >  89.0f) pitch =  89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}








// two-finger scroll to zoom (narrows the FOV)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)  fov = 1.0f;
    if (fov > 90.0f) fov = 90.0f;
}







// triangle fan: hub at the center, then segments+1 rim points
// (the last one repeats the first to close the circle)
std::vector<float> generateCircle(float cx, float cy, float radius, int segments) {
    std::vector<float> verts;
    verts.reserve((segments + 2) * 3);

    verts.push_back(cx);
    verts.push_back(cy);
    verts.push_back(0.0f);

    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * (float)M_PI * i / segments;
        verts.push_back(cx + radius * cosf(angle));
        verts.push_back(cy + radius * sinf(angle));
        verts.push_back(0.0f);
    }

    return verts;
}

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
