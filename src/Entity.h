
#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Entity {

    public:
            unsigned int VAO, VBO, EBO;
            bool VAOInitialized = false;
            std::vector<float> position;
            float mass = 0;
            glm::vec3 velocity = {0.0f, 0.0f, 0.0f};

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
                        verts.push_back(ringRadius * cosf(angle) + pos[0]);
                        verts.push_back(ringRadius * sinf(angle) + pos[1]);
                        verts.push_back(height + pos[2]);
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

            glm::mat4 getPosAfterTranslation(glm::vec3 translationVector, glm::mat4 &matrix) {
                std::vector<float> translation {translationVector.x, translationVector.y, translationVector.z};

                std::vector<float> pos = position;

                for (int i = 0; i <= 2; ++i){
                    pos[i] += translation[i];
                }

                matrix = glm::translate(matrix, glm::make_vec3(pos.data()));

                glm::vec4 c = matrix * glm::vec4{pos[0], pos[1], pos[2], 1.0f};
                return matrix;
            }

            void printPos (glm::mat4 matrix){
                auto pos = std::vector<float> {matrix[3][0], matrix[3][1], matrix[3][2]};

                for (auto i = pos.begin(); i < pos.end(); ++i){
                    if (i == pos.begin()) std::cout << '[';
                    std::cout << *i;
                    if (i != pos.end() - 1) std::cout << ", ";
                    else std::cout << ']' << std::endl;
                }
            }

            void setMass (float objectMass){
                mass = objectMass;
            }

            /* 
             * implementation of change in velocity due to gravity
             * Going to have to calc velocity using most basic function
             * a = G * mass / r^2
             * then use accel to find change in velocity
             * dv = a * dt     (change in vel = accel * change in time)
             * dx = v * dt     (change in pos = vel * change in time)
             *
             * So if i get the distance of the two objects by subtracting their pos's
             * Then, I divide that distance by itself to get a vector with the direction of movement
             * I can then calculate the change in velocity using this vector multiplied by the strength (calc above)
             * Then, I multiply by delta time to get delta velocity
             * Then once again by delta time to get change in position. 
             * Then just add that to the position vector too get the new position.
             */

            glm::vec3 calcPosDueToGrav(glm::vec3 attractorPos, glm::vec3 objectPos, float massOfAttractor, float deltaTime) {

                const float gravity = 1.0f;

                glm::vec3 direction = attractorPos - objectPos;
                // this returns the direction of the movement in vector form
                
                auto r = glm::length(direction);
                
                auto accel = (gravity * massOfAttractor) / (r * r);
                auto deltaVelocity = accel * deltaTime;
                velocity += (direction * deltaVelocity);

                auto deltaPos = velocity * deltaTime;

                for (int i = 0; i <=2; ++i){
                    std::cout << deltaVelocity << std::endl;
                }

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

