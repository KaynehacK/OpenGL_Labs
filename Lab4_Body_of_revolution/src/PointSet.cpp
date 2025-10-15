#include "PointSet.h"
#include "Shader.h"
#include <GL/glew.h>

PointSet::PointSet() : VAO(0), VBO(0), buffersGenerated(false) {}

PointSet::~PointSet() {
    if (buffersGenerated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void PointSet::addPoint(float x, float y) {
    points.push_back(glm::vec3(x, y, 0.0f));
}

void PointSet::addPoint(const glm::vec3& point) {
    points.push_back(point);
}

const std::vector<glm::vec3>& PointSet::getPoints() const {
    return points;
}

size_t PointSet::getNumPoints() const {
    return points.size();
}

void PointSet::clearPoints() {
    points.clear();
    if (buffersGenerated) {
        updateBuffers();
    }
}

void PointSet::setupBuffers() {
    if (points.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffersGenerated = true;
}

void PointSet::updateBuffers() {
    if (!buffersGenerated && !points.empty()) {
        setupBuffers();
        return;
    }
    if (!buffersGenerated && points.empty()){
        return;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!points.empty()) {
        glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void PointSet::Draw(Shader& shader) {
    if (points.empty() || !buffersGenerated) return;

    shader.Use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
    glBindVertexArray(0);
} 
