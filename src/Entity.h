#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


class Entity {

    public:
            unsigned int VAO, VBO, EBO;
            bool VAOInitialized = false;

            void genBufferObject(std::vector<float> verts, 
                    GLenum usage, bool ElementBufferCheck, 
                    std::vector<unsigned int> indices = std::vector<unsigned int> {0}){

                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                if (ElementBufferCheck) glGenBuffers(1, &EBO);
                
                glBindVertexArray(VAO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                
                
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), 
                        verts.data(), usage);

                if (ElementBufferCheck) {
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                            indices.data(), usage);
                }

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);
            }
};


//auto calcAccel = [](float force, float mass){ return force/mass; };
//
//auto calcNewVelocity = [](float oldV, float accel, float delta){
//    return oldV + (accel * delta); 
//};
//
//auto calcNewPos = [](float oldPos, float velocity, float delta){
//    return oldPos + (velocity * delta); 
//};
//
//float calcPosFromGravity (float G, float mass1, float mass2, float radius){
//    float force = (G * mass1 * mass2) / (radius * radius);
//
//    float accel = force / mass1;
//
//    float velocity = 
//}
