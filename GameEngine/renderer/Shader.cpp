#include "Shader.h"
#include <iostream>

unsigned int Shader::compileStage(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cout << "Shader compile error: " << log << std::endl;
    }
    return shader;
}

bool Shader::compile(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vert = compileStage(GL_VERTEX_SHADER, vertexSrc);
    unsigned int frag = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);

    m_program = glCreateProgram();
    glAttachShader(m_program, vert);
    glAttachShader(m_program, frag);
    glLinkProgram(m_program);

    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(m_program, 512, nullptr, log);
        std::cout << "Shader link error: " << log << std::endl;
        return false;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return true;
}

void Shader::use() const { glUseProgram(m_program); }
void Shader::destroy() { glDeleteProgram(m_program); }

void Shader::setMat4(const char* name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_program, name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setVec3(const char* name, const glm::vec3& v) const {
    glUniform3fv(glGetUniformLocation(m_program, name), 1, glm::value_ptr(v));
}
void Shader::setFloat(const char* name, float val) const {
    glUniform1f(glGetUniformLocation(m_program, name), val);
}
void Shader::setInt(const char* name, int val) const {
    glUniform1i(glGetUniformLocation(m_program, name), val);
}