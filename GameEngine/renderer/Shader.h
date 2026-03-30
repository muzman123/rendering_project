#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {
public:
    bool compile(const char* vertexSrc, const char* fragmentSrc);
    void use() const;
    void destroy();

    void setMat4(const char* name, const glm::mat4& mat) const;
    void setVec3(const char* name, const glm::vec3& v) const;
    void setFloat(const char* name, float val) const;
    void setInt(const char* name, int val) const;

    unsigned int getID() const { return m_program; }

private:
    unsigned int m_program = 0;
    unsigned int compileStage(unsigned int type, const char* source);
};