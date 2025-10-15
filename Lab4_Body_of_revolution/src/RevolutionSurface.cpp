#include "RevolutionSurface.h"
#include "Shader.h"
#include <GL/glew.h>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RevolutionSurface::RevolutionSurface() : VAO(0), VBO(0), EBO(0), buffersGenerated(false) {}

RevolutionSurface::~RevolutionSurface() {
    if (buffersGenerated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
}

void RevolutionSurface::generateSurface(const std::vector<glm::vec3>& profileCurvePoints, int numSegments, char axis) {
    vertices.clear();
    indices.clear();

    if (profileCurvePoints.size() < 2 || numSegments < 3) {
        std::cerr << "RevolutionSurface: Not enough points in profile curve or too few segments." << std::endl;
        if(buffersGenerated) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        return;
    }

    float angleStep = 2.0f * static_cast<float>(M_PI) / numSegments;

    for (size_t i = 0; i < profileCurvePoints.size(); ++i) {
        const glm::vec3& p = profileCurvePoints[i];

        for (int j = 0; j <= numSegments; ++j) {
            float currentAngle = j * angleStep;
            Vertex v;

            if (axis == 'X') {
                v.Position.x = p.x;
                v.Position.y = p.y * cos(currentAngle);
                v.Position.z = p.y * sin(currentAngle);
            } else if (axis == 'Y') {
                v.Position.x = p.x * cos(currentAngle);
                v.Position.y = p.y;
                v.Position.z = -p.x * sin(currentAngle);
            } else {
                v.Position.x = p.x * cos(currentAngle) - p.y * sin(currentAngle);
                v.Position.y = p.x * sin(currentAngle) + p.y * cos(currentAngle);
                v.Position.z = p.z;
                 v.Position.x = p.x;
                 v.Position.y = p.y * cos(currentAngle);
                 v.Position.z = p.y * sin(currentAngle);
            }
            glm::vec3 normal_radial_component;
            glm::vec3 tangent_profile_approx;

            if (axis == 'X') {
                normal_radial_component = glm::vec3(0.0f, v.Position.y, v.Position.z);
                glm::vec3 p_prev = (i > 0) ? profileCurvePoints[i-1] : p;
                glm::vec3 p_next = (i < profileCurvePoints.size() - 1) ? profileCurvePoints[i+1] : p;
                glm::vec3 dp = p_next - p_prev;

                tangent_profile_approx.x = dp.x;
                tangent_profile_approx.y = dp.y * cos(currentAngle);
                tangent_profile_approx.z = dp.y * sin(currentAngle);
                glm::vec3 tangent_circle = glm::vec3(0, -p.y * sin(currentAngle), p.y * cos(currentAngle));
                v.Normal = glm::normalize(glm::cross(glm::normalize(tangent_profile_approx), glm::normalize(tangent_circle)));
                 if (glm::dot(v.Normal, normal_radial_component) < 0) {
                    v.Normal = v.Normal * -1.0f;
                 }

            } else if (axis == 'Y') {
                normal_radial_component = glm::vec3(v.Position.x, 0.0f, v.Position.z);
                glm::vec3 p_prev = (i > 0) ? profileCurvePoints[i-1] : p;
                glm::vec3 p_next = (i < profileCurvePoints.size() - 1) ? profileCurvePoints[i+1] : p;
                glm::vec3 dp = p_next - p_prev;

                tangent_profile_approx.x = dp.x * cos(currentAngle);
                tangent_profile_approx.y = dp.y;
                tangent_profile_approx.z = -dp.x * sin(currentAngle);

                glm::vec3 tangent_circle = glm::vec3(-p.x * sin(currentAngle), 0, -p.x * cos(currentAngle));
                v.Normal = glm::normalize(glm::cross(glm::normalize(tangent_circle), glm::normalize(tangent_profile_approx))); // Порядок важен для направления
                if (glm::dot(v.Normal, normal_radial_component) < 0) {
                    v.Normal = v.Normal * -1.0f;
                 }
            }
            vertices.push_back(v);
        }
    }

    for (size_t i = 0; i < profileCurvePoints.size() - 1; ++i) {
        for (int j = 0; j < numSegments; ++j) {
            unsigned int idx0 = i * (numSegments + 1) + j;
            unsigned int idx1 = i * (numSegments + 1) + (j + 1);
            unsigned int idx2 = (i + 1) * (numSegments + 1) + j;
            unsigned int idx3 = (i + 1) * (numSegments + 1) + (j + 1);

            indices.push_back(idx0);
            indices.push_back(idx2);
            indices.push_back(idx1);

            indices.push_back(idx1);
            indices.push_back(idx2);
            indices.push_back(idx3);
        }
    }
}


void RevolutionSurface::setupBuffers() {
    if (vertices.empty() || indices.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffersGenerated = true;
}

void RevolutionSurface::Draw(Shader& shader, const glm::mat4& modelMatrix) {
    if (vertices.empty() || indices.empty() || !buffersGenerated) return;

    shader.Use();
    shader.setMat4("model", modelMatrix);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void RevolutionSurface::clearSurface(){
    vertices.clear();
    indices.clear();
    if(buffersGenerated){
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
} 
