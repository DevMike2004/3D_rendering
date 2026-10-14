#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Camera {
    public: 

        glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

        float yaw   = -90.0f;   // -90 so we start looking down -Z, not +X
        float pitch = 0.0f;

        bool  firstMouse = true;

        // --- timing -----------------------------------------------------------------
        float deltaTime = 0.0f;   // seconds since last frame
        float lastFrame = 0.0f;

        int WIN_WIDTH , WIN_HEIGHT;
        float fov;

        float lastX;
        float lastY;

        Camera(float camFov, int width, int height){
            fov = camFov;
            WIN_HEIGHT = height;
            WIN_WIDTH = width;

            float lastX = WIN_WIDTH / 2.0f;
            float lastY = WIN_HEIGHT / 2.0f;
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
        void processMouse(double xpos, double ypos) {

            // first event is a huge jump from the default (0,0) -- swallow it
            if (firstMouse) {
                lastX = (float)xpos;
                lastY = (float)ypos;
                firstMouse = false;
            };

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
        };

        // two-finger scroll to zoom (narrows the FOV)
        void processScroll(double xoffset, double yoffset) {
            fov -= (float)yoffset;
            if (fov < 1.0f)  fov = 1.0f;
            if (fov > 90.0f) fov = 90.0f;
        };
};
