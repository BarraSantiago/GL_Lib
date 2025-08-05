#include "SpotLight.h"
    #include <cmath>
    
    namespace gllib
    {
        SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const Color& color,
                             float innerCutOff, float outerCutOff, float constant, float linear, float quadratic)
            : Light(color), position(position), direction(glm::normalize(direction)),
              innerCutOff(innerCutOff), outerCutOff(outerCutOff),
              constant(constant), linear(linear), quadratic(quadratic)
        {
        }
    
        glm::vec3 SpotLight::getPosition() const
        {
            return position;
        }
    
        void SpotLight::setPosition(const glm::vec3& newPosition)
        {
            position = newPosition;
        }
    
        glm::vec3 SpotLight::getDirection() const
        {
            return direction;
        }
    
        void SpotLight::setDirection(const glm::vec3& newDirection)
        {
            direction = glm::normalize(newDirection);
        }
    
        void SpotLight::setCutOff(float newInnerCutOff, float newOuterCutOff)
        {
            innerCutOff = newInnerCutOff;
            outerCutOff = newOuterCutOff;
        }
    
        float SpotLight::getInnerCutOff() const
        {
            return innerCutOff;
        }
    
        float SpotLight::getOuterCutOff() const
        {
            return outerCutOff;
        }
    
        void SpotLight::setAttenuation(float newConstant, float newLinear, float newQuadratic)
        {
            constant = newConstant;
            linear = newLinear;
            quadratic = newQuadratic;
        }
    
        void SpotLight::apply(unsigned int shaderProgram) const
        {
            // Set spotlight position and direction
            Shader::setVec3(shaderProgram, "spotLight.position", position.x, position.y, position.z);
            Shader::setVec3(shaderProgram, "spotLight.direction", direction.x, direction.y, direction.z);
    
            // Set spotlight color
            Shader::setVec3(shaderProgram, "spotLight.color", color.r, color.g, color.b);
    
            // Set spotlight cone angles (convert to cosine for shader efficiency)
            Shader::setFloat(shaderProgram, "spotLight.innerCutOff", cos(glm::radians(innerCutOff)));
            Shader::setFloat(shaderProgram, "spotLight.outerCutOff", cos(glm::radians(outerCutOff)));
    
            // Set attenuation factors
            Shader::setFloat(shaderProgram, "spotLight.constant", constant);
            Shader::setFloat(shaderProgram, "spotLight.linear", linear);
            Shader::setFloat(shaderProgram, "spotLight.quadratic", quadratic);
    
            // Set light strengths
            Shader::setFloat(shaderProgram, "spotLight.diffuseStrength", 1.0f);
            Shader::setFloat(shaderProgram, "spotLight.specularStrength", 0.5f);
        }
    }