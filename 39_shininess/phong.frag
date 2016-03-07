#version 150

in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;

//CREATE STRUCTS (CONTAINERS) FOR THE MATERIAL PROPERTIES AND LIGHT PROPERTIES
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec4 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    
    // Ambient
    vec3 ambient = light.ambient * material.ambient;
    
    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light.position - vPosition.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);
    
    // Specular
    vec3 viewDir = normalize(viewPos.xyz - vPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);
    
    vec3 result = ambient + diffuse + specular;
    
    oColor = vec4(result, 1.0f);
    
    
}