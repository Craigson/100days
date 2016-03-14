

uniform mat4 ciViewMatrix;


in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;

struct Light {
    
    vec3 position;
    vec3 direction;
    float innerCutOff;
    float outerCutOff;
    
    float constant;
    float linear;
    float quadratic;
};

uniform Light light;


void main()
{


    //THE LIGHT DIRECTION
    vec3 lightDir = normalize(light.position - vPosition.xyz);

    // Ambient
    vec3 ambient = vec3(0.03f);;
    
    // Diffuse
    vec3 norm = normalize(vNormal);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = vec3(0.8f) * diff ;
    
    // Specular
    vec3 viewDir = normalize(-vPosition.xyz);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.f);
    vec3 specular = vec3(1.f) * spec;

    
    //CALCULATE THETA (THE ANGLE BETWEEN THE LIGHT VECTOR AND THE INNER CONE) AND EPSILON ( THE ANGLE BETWEEN THE SPOTLIGHT VECTOR AND A NEW OUTER CONE )
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutOff - light.outerCutOff;
    
    //THE INTENSITY IS A FUNCTION OF HOW FAR THE FRGMENT IS FROM THE LIGHT DIRECTION, IF IT IS WITHIN THETA, IT IS FULL BRIGHTNESS, IF IT IS BETWEEN THETA AND EPSILON IT IS A FUNCTION OF HOW FAR IT IS (GETTING DIMMER AS IT MOVES AWAY), IF IT'S OUTSIDE THE OUTER CONE, THERE'LL BE NO LIGHT AT ALL
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        
    
        
        // Attenuation
        float distance    = length(light.position - vPosition.xyz);
        float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * (distance * distance));
        
        diffuse  *= attenuation;
        specular *= attenuation;
        
        diffuse  *= intensity;
        specular *= intensity;
    
        
        oColor = vec4(ambient + diffuse + specular, 1.0f);
//    oColor = vec4(1.,1.,1.,1.);
    
    
}
