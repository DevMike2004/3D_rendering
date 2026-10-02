#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Entity {
    public:

        unsigned int VAO, VBO, EBO;
        bool VAOInitialized = false;

        void genVertexBufferObject(std::vector<float> verts, GLenum usage) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float),
                         verts.data(), usage);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            VAOInitialized = true;
        };

        void genElementBufferObject(std::vector<float> verts, std::vector<unsigned int> indices
                , GLenum usage) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);
            
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), 
                    verts.data(), usage);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                    indices.data(), usage);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
        }
};
