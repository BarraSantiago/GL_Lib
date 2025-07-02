#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    bool hasTexture;
};

struct Light {
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    
    float innerCutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
    
    float diffuseStrength;
    float specularStrength;
};

uniform Material material;
uniform Light light;
uniform SpotLight spotLight;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 ambientStrength;
uniform float diffuseStrength = 1.0; 
uniform float specularStrength = 1.0;

vec3 calcPointLight(vec3 lightPos, vec3 lightColor, Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Get material colors
    vec3 diffuseColor = material.hasTexture ? texture(material.texture_diffuse1, TexCoords).rgb : material.diffuse;
    vec3 specularColor = material.hasTexture ? texture(material.texture_specular1, TexCoords).rgb : material.specular;
    
    // Combine results
    vec3 diffuse = diffuseStrength * diff * lightColor * diffuseColor;
    vec3 specular = specularStrength * spec * lightColor * specularColor;
    
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (diffuse + specular);
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // Check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    // Get material colors
    vec3 diffuseColor = material.hasTexture ? texture(material.texture_diffuse1, TexCoords).rgb : material.diffuse;
    vec3 specularColor = material.hasTexture ? texture(material.texture_specular1, TexCoords).rgb : material.specular;
    
    // Combine results
    vec3 diffuse = light.diffuseStrength * diff * light.color * diffuseColor;
    vec3 specular = light.specularStrength * spec * light.color * specularColor;
    
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (diffuse + specular);
}

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Get material colors
    vec3 diffuseColor = material.hasTexture ? texture(material.texture_diffuse1, TexCoords).rgb : material.diffuse;
    
    // Ambient lighting
    vec3 ambient = ambientStrength * diffuseColor;
    
    // Point light contribution
    vec3 pointLightContrib = calcPointLight(lightPos, lightColor, light, norm, FragPos, viewDir);
    
    // Spotlight contribution
    vec3 spotLightContrib = calcSpotLight(spotLight, norm, FragPos, viewDir);
    
    vec3 result = ambient + pointLightContrib + spotLightContrib;
    FragColor = vec4(result, 1.0);
}