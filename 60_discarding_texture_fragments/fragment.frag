#version 150

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vColor;

uniform sampler2D uTex;

out vec4 oColour;

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

#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

/////////////////////// Define Functions ////////////////////
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);



void main(void)
{
    
    // Properties
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize( - vPosition.xyz);
    
    vec3 result = vec3(0);
    
    // ADD THE COMPONENTS FROM THE DIFFERENT POINT LIGHTS
    for(int i = 0; i < NR_POINT_LIGHTS; i++) result += CalcPointLight(pointLights[i], norm, vPosition.xyz, viewDir);
    
    //IF THE ALPHA VALUE OF THE FRAGMENT AT THE CURRENT TEXTURE COORDINATE IS TRANSPARENT, DISCARD THAT FRAGMENT
    vec4 texColor = texture(uTex, vTexCoord);
    if(texColor.a < 0.1)
        discard;
    
    oColour = vec4(result, 1.0);
    
}


/////////////////////// CalcPointLight /////////////////////

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.f);
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