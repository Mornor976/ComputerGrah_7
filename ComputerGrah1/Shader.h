#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glew.h>
#include "glm/glm.hpp"

class Shader {
public:
    GLuint Program;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
    void Use();
    void SetUniform(const std::string& name, float value);
    void SetUniform(const std::string& name, int value);
    void SetUniform(const std::string& name, float x, float y, float z);
    void SetUniform(const std::string& name, float r, float g, float b, float a);
    void SetUniform(const std::string& name, float* matrix);
 

private:
    std::string ReadFile(const GLchar* filename);
};

#endif