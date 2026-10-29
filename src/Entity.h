#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Entity {

    public:
            unsigned int VAO, VBO, EBO;
            bool VAOInitialized = false;
            std::vector<float> position;

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

            std::vector<float> generateSphere(std::vector<float> pos, float radius, int segments) {
                
                position = pos;

                std::vector<float> verts;
                verts.reserve((segments + 1) * (segments /2 + 1) * 3);

                for (int i = 0; i <= segments/2; ++i){
                    float stackAngle = (float)M_PI * i / (segments / 2.0f);
                    float ringRadius = radius * sin(stackAngle);
                    float height = radius * cos(stackAngle);

                    for (int j = 0; j <= segments; ++j) {
                        float angle = 2.0f * (float)M_PI * j / segments;
                        verts.push_back(ringRadius * cosf(angle) + pos[0]);
                        verts.push_back(ringRadius * sinf(angle) + pos[1]);
                        verts.push_back(height + pos[2]);
                    }
                }

                return verts;
            }
            std::vector<float> makeGrid(int cellsPerSide, float cellSize) {
                std::vector<float> v;
                float half = cellsPerSide * cellSize * 0.5f;

                for (int i = 0; i <= cellsPerSide; ++i) {
                    float p = -half + i * cellSize;

                    // line parallel to Z
                    v.insert(v.end(), { p, -3.0f, -half,   p, -3.0f,  half });
                    // line parallel to X
                    v.insert(v.end(), { -half, -3.0f, p,   half, -3.0f, p });
                }
                return v;
            }

            std::vector<float> getPos(std::vector<float> positionVector, glm::vec3 translationVector) {
                std::vector<float> translation {translationVector.x, translationVector.y, translationVector.z};
                for (int i = 0; i <= 2; ++i){
                    positionVector[i] = positionVector[i] + translation[i];
                }
                return positionVector;
            }
};

// *** work in progress ***

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
