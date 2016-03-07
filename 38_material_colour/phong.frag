#version 150

uniform vec3 uLightPos;
uniform vec3 uCam;

uniform vec3 uLightColor;
uniform vec3 uObjectColor;


in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;

void main()
{
    
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * uLightColor;
    
    // Diffuse
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(uLightPos - vPosition.xyz);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;
    
    // Specular
    float specularStrength = 0.9f;
    vec3 viewDir = normalize(uCam - vPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512);
    vec3 specular = specularStrength * spec * uLightColor;
    
    vec3 result = (ambient + diffuse + specular) * uObjectColor;
    oColor = vec4(result, 1.0f);
    
    
}