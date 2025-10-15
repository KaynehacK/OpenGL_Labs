#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

class Curve {
public:
    unsigned int VAO, VBO;
    std::vector<glm::vec3> curvePoints;

    Curve();
    ~Curve();

    void generateCurve(const std::vector<glm::vec3>& controlPoints, int segmentsPerControlPoint = 10);
    
    void setupBuffers();
    void updateBuffers();
    void Draw(Shader& shader);
    void clearCurve();

private:
    bool buffersGenerated = false;
};

#endif 
