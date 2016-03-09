#version 150

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord0;


out vec4 oColor;


//CREATE STRUCTS (CONTAINERS) FOR THE MATERIAL PROPERTIES AND LIGHT PROPERTIES
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
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
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vTexCoord0)); //set ambient colour equal to diffuse material's color
    //vec3 ambient = vec3(0.05f);
    
    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(light.position - vPosition.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vTexCoord0)); //sample from the texture to retrieve fragment's diffuse component
    
    // Specular
    vec3 viewDir = normalize(viewPos.xyz - vPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3 specular = light.specular * (spec * material.specular);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vTexCoord0)); //set specular color component according to specular map
    
    vec3 result = ambient + diffuse + specular;
    
    oColor = vec4(result, 1.0f);
    
    
}