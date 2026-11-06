
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Entity {

    public:
            unsigned int VAO, VBO, EBO = NAN;
            bool VAOInitialized = false;
            float mass = 0;

            glm::mat4 model = glm::mat4(1.0f);
            std::vector<float> position;
            glm::vec3 velocity = {0.0f, 0.0f, 0.0f};
            float accel = 0;

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

            std::vector<float> generateSphere(std::vector<float> pos, float radius, int segments, float objectMass) {
                
                position = pos;

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
                mass = objectMass;

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

            void printPosAfterTranslation(glm::vec3 translationVector, glm::mat4 matrix) {
                std::vector<float> translation {translationVector.x, translationVector.y, translationVector.z};

                std::vector<float> pos = position;

                matrix = glm::translate(matrix, glm::make_vec3(pos.data()));

                pos = std::vector<float> {matrix[3][0], matrix[3][1], matrix[3][2]};

                for (auto i = pos.begin(); i < pos.end(); ++i){
                    if (i == pos.begin()) std::cout << '[';
                    std::cout << *i;
                    if (i != pos.end() - 1) std::cout << ", ";
                    else std::cout << ']' << std::endl;
                }
            }

            void getPosAfterTranslation(glm::vec3 translationVector, glm::mat4 &matrix) {
                std::vector<float> translation {translationVector.x, translationVector.y, translationVector.z};

                std::vector<float> pos = position;

                matrix = glm::translate(matrix, glm::make_vec3(pos.data()));

                pos = std::vector<float> {matrix[3][0], matrix[3][1], matrix[3][2]};

            }

            void setMass (float objectMass){
                mass = objectMass;
            }

            glm::vec3 calcAccelDueToGrav(glm::vec3 attractorPos, glm::vec3 objectPos, float massOfAttractor) {

                const float gravity = 1.0f;

                // distance between the 2 objects
                glm::vec3 distance = attractorPos - objectPos;
                
                // the length of the distance itself (non vector form)
                auto r = glm::length(distance);
                // the direction of the objects movements
                auto dir = distance / r;
                
                return dir * ((gravity * massOfAttractor) / (r * r + 0.1f));
             }

            glm::vec3 calcPosFromAccel(glm::vec3 accel, float deltaTime){
                velocity += accel * deltaTime;

                auto deltaPos = velocity * deltaTime;

                glm::vec3 pos = glm::make_vec3(position.data()) += deltaPos;
                position[0] = pos[0];
                position[1] = pos[1];
                position[2] = pos[2];

                return deltaPos;
            }

            void setVelocity(float velX, float velY, float velZ){
                velocity[0] = velX;
                velocity[1] = velY;
                velocity[2] = velZ;
            }


};
// functions to orbit around a spot in space
// will be using later in more advanced orbiting functions
        //float sphereX = (float)cosf(currentFrame) * orbitRadius;
        //float sphereZ = (float)sinf(currentFrame) * orbitRadius;
        // glm::vec3 sphereTranslate = glm::vec3(sphereX, 0.0f, sphereZ);
