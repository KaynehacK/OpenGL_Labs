#ifndef REVOLUTION_SURFACE_H
#define REVOLUTION_SURFACE_H

#include <vector>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
};

class RevolutionSurface {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    RevolutionSurface();
    ~RevolutionSurface();

    void generateSurface(const std::vector<glm::vec3>& curvePoints, int numSegments, char axis = 'X');
    void calculateNormals();

    void setupBuffers();
    void Draw(Shader& shader, const glm::mat4& modelMatrix);
    void clearSurface();

private:
    bool buffersGenerated = false;
};

#endif 
