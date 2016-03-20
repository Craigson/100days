#version 150

in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;


///////////////////////// DirLight  ///////////////////////
struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirLight dirLight;

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
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform vec3 uCamPosition;

/////////////////////// Define Functions ////////////////////

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


//////////////////////// main ///////////////////////////////

void main(void)
{
    
    // Properties
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(uCamPosition - vPosition.xyz);
    
    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir);
    // Phase 2: Point lights
//    vec3 result = vec3(0.);
    for(int i = 0; i < NR_POINT_LIGHTS; i++) result += CalcPointLight(pointLights[i], norm, vPosition.xyz, viewDir);
    // Phase 3: Spot light
    //result += CalcSpotLight(spotLight, norm, vPosition.xyz, viewDir);
    
    oColor = vec4(result, 1.0);
}


////////////////////// CalcDirLight ///////////////////////

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.f);
    // Combine results
    vec3 ambient  = light.ambient;
    vec3 diffuse  = light.diffuse;
    vec3 specular = light.specular * spec;
    return (ambient + diffuse + specular);
}

/////////////////////// CalcPointLight /////////////////////

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = normalize(reflect(-lightDir, normal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.f);
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
