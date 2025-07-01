#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0f);
}

#shader fragment
#version 330 core

#define MAX_LIGHTS 8

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform Material material;
uniform Light[MAX_LIGHTS] lights;
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
    vec3 ambient = dirLight.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(dirLight.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dirLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = dirLight.specular * spec * texture(material.specular, TexCoords).rgb;

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(Light pointLight)
{
    vec3 ambient = pointLight.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = pointLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.specular * spec * texture(material.specular, TexCoords).rgb;

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.attenuation.x + pointLight.attenuation.y * distance + pointLight.attenuation.z * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 calculateSpotLight(Light spotLight)
{
    vec3 ambient = spotLight.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(spotLight.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = spotLight.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spotLight.specular * spec * texture(material.specular, TexCoords).rgb;

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