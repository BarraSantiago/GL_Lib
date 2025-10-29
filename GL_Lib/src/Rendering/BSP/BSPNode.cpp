#include "BSPNode.h"
#include "Camera.h"
#include "Frustum.h"
#include "Importer/Model.h"
#include <iostream>

namespace gllib
{
    void BSPNode::addModel(Model* model)
    {
        if (isLeaf())
        {
            models.push_back(model);
            return;
        }

        glm::vec3 modelCenter = (model->transform.getWorldAABBMin() + model->transform.getWorldAABBMax()) * 0.5f;

        if (plane.isPointInFront(modelCenter))
        {
            if (!frontChild)
                frontChild = std::make_unique<BSPNode>();
            frontChild->addModel(model);
        }
        else
        {
            if (!backChild)
                backChild = std::make_unique<BSPNode>();
            backChild->addModel(model);
        }
    }

    void BSPNode::collectVisibleModels(const glm::vec3& cameraPos, const Frustum& frustum,
                                       std::vector<Model*>& visibleModels)
    {
        if (isLeaf())
        {
            for (Model* model : models)
            {
                if (model)
                {
                    // Always add models to visible list if they have any part on camera's side
                    // The model's own hierarchical rendering will handle per-mesh culling
                    visibleModels.push_back(model);
                }
            }
            return;
        }
    
        bool cameraInFront = plane.isPointInFront(cameraPos);
    
        // Only traverse the side where the camera is
        if (cameraInFront)
        {
            if (frontChild)
                frontChild->collectVisibleModels(cameraPos, frustum, visibleModels);
        }
        else
        {
            if (backChild)
                backChild->collectVisibleModels(cameraPos, frustum, visibleModels);
        }
    }

    BSPPlaneVisualizer::BSPPlaneVisualizer(const BSPPlane& plane, float size)
    {
        // Calculate a point on the plane
        glm::vec3 planePoint = plane.normal * -plane.distance;

        // Find two perpendicular vectors to the normal
        glm::vec3 tangent, bitangent;
        if (std::abs(plane.normal.x) > 0.9f)
        {
            tangent = glm::normalize(glm::cross(plane.normal, glm::vec3(0.0f, 1.0f, 0.0f)));
        }
        else
        {
            tangent = glm::normalize(glm::cross(plane.normal, glm::vec3(1.0f, 0.0f, 0.0f)));
        }
        bitangent = glm::normalize(glm::cross(plane.normal, tangent));

        // Create a grid on the plane
        glm::vec3 color(1.0f, 1.0f, 0.0f); // Yellow color

        // Create grid lines
        int gridLines = 10;
        float step = size / gridLines;

        for (int i = -gridLines; i <= gridLines; i++)
        {
            float offset = i * step;

            // Lines along tangent
            glm::vec3 start = planePoint + tangent * (-size) + bitangent * offset;
            glm::vec3 end = planePoint + tangent * size + bitangent * offset;

            vertices.push_back(start.x);
            vertices.push_back(start.y);
            vertices.push_back(start.z);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
            vertices.push_back(end.x);
            vertices.push_back(end.y);
            vertices.push_back(end.z);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);

            // Lines along bitangent
            start = planePoint + bitangent * (-size) + tangent * offset;
            end = planePoint + bitangent * size + tangent * offset;

            vertices.push_back(start.x);
            vertices.push_back(start.y);
            vertices.push_back(start.z);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
            vertices.push_back(end.x);
            vertices.push_back(end.y);
            vertices.push_back(end.z);
            vertices.push_back(color.r);
            vertices.push_back(color.g);
            vertices.push_back(color.b);
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    BSPPlaneVisualizer::~BSPPlaneVisualizer()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    void BSPPlaneVisualizer::draw(const glm::mat4& view, const glm::mat4& projection)
    {
        // Use a simple shader for debug visualization
        glm::mat4 mvp = projection * view;

        GLint currentProgram;
        glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

        GLint mvpLoc = glGetUniformLocation(currentProgram, "u_MVP");
        if (mvpLoc != -1)
        {
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        }

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, vertices.size() / 6);
        glBindVertexArray(0);
    }
}
