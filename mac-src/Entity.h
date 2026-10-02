#include<vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Entity {
    public:

        unsigned int genVertexBufferObject(std::vector<float> verts, 
                GLenum drawMode, GLenum usage) {
            unsigned int VAO, VBO;
            GLenum drawMode, usage = 

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float),
                         verts.data(), GL_STATIC_DRAW);

        };


        

};
