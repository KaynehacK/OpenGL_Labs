#ifndef POINT_SET_H
#define POINT_SET_H

#include <vector>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class PointSet {
public:
    std::vector<glm::vec3> points;
    unsigned int VAO, VBO;

    PointSet();
    ~PointSet();

    void addPoint(float x, float y);
    void addPoint(const glm::vec3& point);
    const std::vector<glm::vec3>& getPoints() const;
    size_t getNumPoints() const;
    void clearPoints();

    void setupBuffers();
    void updateBuffers();
    void Draw(Shader& shader);

private:
    bool buffersGenerated = false;
};

#endif 
