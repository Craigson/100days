

uniform mat4 ciViewMatrix;


in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;

struct Light {
    
    vec3 position;
    vec3 direction;
    float cutOff;
    
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;


void main()
{
    
    //THE LIGHT DIRECTION 
    vec3 lightDir = normalize(light.position - vPosition.xyz);
    
    // Check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction));
    
    if(theta > light.cutOff) // Remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {
        // Ambient
        vec3 ambient = vec3(0.1f);;
        
        // Diffuse
        vec3 norm = normalize(vNormal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = vec3(0.8f) * diff ;
        
        // Specular
        vec3 viewDir = normalize(-vPosition.xyz);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.f);
        vec3 specular = vec3(1.f) * spec;
        
        
        // Attenuation
        float distance    = length(light.position - vPosition.xyz);
        float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * (distance * distance));
        
        diffuse  *= attenuation;
        specular *= attenuation;
    
        
        oColor = vec4(ambient + diffuse + specular, 1.0f);
    }
    else    // else, use ambient light so scene isn't completely dark outside the spotlight.
        oColor = vec4(vec3(0.03f), 1.f);
    
    
}
