#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader{
    public:
        unsigned int ID;

        Shader (const char* vertexPath, const char* fragmentPath){
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;

            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            try{
                // this is the creation of the file handlers
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;

                // here we are inputting a read buffer into the read streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();

                // then we close the handlers
                vShaderFile.close();
                fShaderFile.close();

                // now we are setting the values for the shader strings to store the juicy stuff
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();


                // basic catch statement to catch shader read failure
            }catch(std::ifstream::failure e){
                std::cout << "ERROR::SHADER::FILE_NOTSUCCESFULLY_READ" << std::endl;
            }

            // we are now creating the shaders and setting the juicy stuff to usable char*'s'
            const char* vShaderCode = vertexCode.c_str();
            const char* fShaderCode = fragmentCode.c_str();

            // below is just the standard procedure of initalizing and compiling the shaders
            unsigned int vertex, fragment;
            int success;
            char infoLog[512];

            vertex = glCreateShader(GL_VERTEX_SHADER);
            fragment = glCreateShader(GL_FRAGMENT_SHADER);

            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glShaderSource(fragment, 1, &fShaderCode, NULL);

            glCompileShader(vertex);
            glCompileShader(fragment);

            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

            if(!success){
                glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };
            if(!success){
                glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            };


            // here we are now attaching and linking the shaders into program ID
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);

            glLinkProgram(ID);

            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if (!success){
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
            }

            glDeleteShader(vertex);
            glDeleteShader(fragment);
        }




        // method to use the shader in our program
        void use(){
            glUseProgram(ID);
        }

        // methods to set the uniforms of the shaders to a desired value
        void setBool(const std::string &name, bool value) const{
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }

        void setInt(const std::string &name, int value) const{
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }

        void setFloat(const std::string &name, float value) const{
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        
        void setMat4(const std::string &name, glm::mat4 value) const{
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1, GL_FALSE, 
                                                            glm::value_ptr(value));
        }

};

#endif
