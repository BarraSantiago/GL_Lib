#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = vec2(aTexCoords.x, aTexCoords.y);
}

#shader fragment
#version 330 core

#define MAX_LIGHTS 8

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D texture_baseColor1;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_height1;
    float shininess;
};

struct Light
{
    int type;
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 attenuation;
    float cutoff;
    float outerCutoff;
};

vec3 calculateDirLight(Light pointLight);
vec3 calculatePointLight(Light pointLight);
vec3 calculateSpotLight(Light spotLight);

uniform Light[MAX_LIGHTS] lights;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    vec3 result = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].type == 0)
        {
            result += calculateDirLight(lights[i]);
        }
        else if (lights[i].type == 1)
        {
            result += calculatePointLight(lights[i]);
        }
        else
        {
            result += calculateSpotLight(lights[i]);
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 calculateDirLight(Light dirLight)
{
    vec3 ambient = dirLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    return ambient + diffuse + specular;
}


vec3 calculatePointLight(Light pointLight)
{
    vec3 ambient = pointLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.attenuation.x + pointLight.attenuation.y * distance + pointLight.attenuation.z * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(Light spotLight)
{
    vec3 ambient = spotLight.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spotLight.specular * spec * texture(material.texture_specular1, TexCoords).rgb;

    float theta = dot(lightDir, normalize(-spotLight.direction));
    float epsilon = (spotLight.cutoff - spotLight.outerCutoff);
    float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0, 1.0);

    diffuse  *= intensity;
    specular *= intensity;

    float distance = length(spotLight.position - FragPos);
    float attenuation = 1.0 / (spotLight.attenuation.x + spotLight.attenuation.y * distance + spotLight.attenuation.z * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}