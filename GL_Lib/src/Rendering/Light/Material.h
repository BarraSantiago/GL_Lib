#pragma once
#include <vec3.hpp>

#include "Core/deps.h"
namespace gllib {
    class Shader;
}
namespace gllib
{
    struct DLLExport Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        Material(
            const glm::vec3& ambient = glm::vec3(0.2f),
            const glm::vec3& diffuse = glm::vec3(0.5f),
            const glm::vec3& specular = glm::vec3(1.0f),
            float shininess = 32.0f)
            : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess)
        {
        }

        void apply(unsigned int shaderProgram) const;

        static Material emerald() { return Material(glm::vec3(0.0215f, 0.1745f, 0.0215f), glm::vec3(0.07568f, 0.61424f, 0.07568f), glm::vec3(0.633f, 0.727811f, 0.633f), 76.8f); }
        static Material jade() { return Material(glm::vec3(0.135f, 0.2225f, 0.1575f), glm::vec3(0.54f, 0.89f, 0.63f), glm::vec3(0.316228f, 0.316228f, 0.316228f), 12.8f); }
        static Material obsidian() { return Material(glm::vec3(0.05375f, 0.05f, 0.06625f), glm::vec3(0.18275f, 0.17f, 0.22525f), glm::vec3(0.332741f, 0.328634f, 0.346435f), 38.4f); }
        static Material pearl() { return Material(glm::vec3(0.25f, 0.20725f, 0.20725f), glm::vec3(1.0f, 0.829f, 0.829f), glm::vec3(0.296648f, 0.296648f, 0.296648f), 11.264f); }
        static Material ruby() { return Material(glm::vec3(0.1745f, 0.01175f, 0.01175f), glm::vec3(0.61424f, 0.04136f, 0.04136f), glm::vec3(0.727811f, 0.626959f, 0.626959f), 76.8f); }
        static Material bronze() { return Material(glm::vec3(0.2125f, 0.1275f, 0.054f), glm::vec3(0.714f, 0.4284f, 0.18144f), glm::vec3(0.393548f, 0.271906f, 0.166721f), 25.6f); }
        static Material gold() { return Material(glm::vec3(0.24725f, 0.1995f, 0.0745f), glm::vec3(0.75164f, 0.60648f, 0.22648f), glm::vec3(0.628281f, 0.555802f, 0.366065f), 51.2f); }
        static Material silver() { return Material(glm::vec3(0.19225f, 0.19225f, 0.19225f), glm::vec3(0.50754f, 0.50754f, 0.50754f), glm::vec3(0.508273f, 0.508273f, 0.508273f), 51.2f); }
        static Material plastic() { return Material(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.55f, 0.55f, 0.55f), glm::vec3(0.70f, 0.70f, 0.70f), 32.0f); }
        static Material base() { return Material(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f), 0.0f); }
    };
}