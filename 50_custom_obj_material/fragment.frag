#version 150

in vec3 vNormal;
in vec4 vPosition;

out vec4 oColor;


///////////////////////// PointLight ///////////////////////

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// USE A PRE-PROCESSSOR DIRECTIVE TO DEFINE THE NUMBER OF POINT LIGHTS
#define NR_POINT_LIGHTS 10
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec4 uCamPosition;

/////////////////////// Define Functions ////////////////////
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main(void)
{
    
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(uCamPosition.xyz - vPosition.xyz);
    
    //  vec3 result = CalcDirLight(dirLight, norm, viewDir);
    vec3 result = vec3(0);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) result += CalcPointLight(pointLights[i], norm, vPosition.xyz, viewDir);

    oColor = vec4(result, 1.0);
    
  //  oColor = vec4(vec3(1.), 1.0);
}

/////////////////////// CalcPointLight /////////////////////

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 512.f);
    // Attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance +
                                light.quadratic * (distance * distance));
    // Combine results
    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse  * diff;
    vec3 specular = light.specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

