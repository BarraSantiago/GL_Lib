#include "Material.h"
#include "Shader.h"

void gllib::Material::apply(unsigned int shaderProgram) const
{
    //Shader::setVec3(shaderProgram, "material.ambient", ambient.x, ambient.y, ambient.z);
    Shader::setVec3(shaderProgram, "material.diffuse", diffuse.x, diffuse.y, diffuse.z);
    Shader::setVec3(shaderProgram, "material.specular", specular.x, specular.y, specular.z);
    Shader::setFloat(shaderProgram, "material.shininess", shininess);
}