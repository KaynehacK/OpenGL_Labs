#include "Curve.h"
#include "Shader.h"
#include <GL/glew.h>

Curve::Curve() : VAO(0), VBO(0), buffersGenerated(false) {}

Curve::~Curve() {
    if (buffersGenerated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void Curve::generateCurve(const std::vector<glm::vec3>& controlPoints, int segmentsPerControlPoint) {
    curvePoints.clear();
    if (controlPoints.size() < 2) {
        if (buffersGenerated) updateBuffers();
        return;
    }

    curvePoints = controlPoints;

    if (buffersGenerated) updateBuffers();
}

void Curve::setupBuffers() {
    if (curvePoints.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(glm::vec3), curvePoints.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffersGenerated = true;
}

void Curve::updateBuffers() {
    if (!buffersGenerated && !curvePoints.empty()) {
        setupBuffers();
        return;
    }
    if(!buffersGenerated && curvePoints.empty()){
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!curvePoints.empty()) {
        glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(glm::vec3), curvePoints.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Curve::Draw(Shader& shader) {
    if (curvePoints.empty() || !buffersGenerated) return;
    shader.Use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(curvePoints.size()));
    glBindVertexArray(0);
}

void Curve::clearCurve(){
    curvePoints.clear();
    if(buffersGenerated){
        updateBuffers();
    }
} 
