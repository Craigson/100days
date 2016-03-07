
uniform vec3 uLightPos;

in vec4 vPosition;
in vec3 vNormal;

out vec4 oColor;

void main()
{
    
    vec3 ambient = vec3(0.1,0.1,0.1); //AMBIENT
    
    //DIFFUSE
    vec3 normal = vNormal;
    
    vec3 lightDir = normalize(uLightPos - vPosition.xyz);
    
    float dist = length(lightDir);
    
    float attenuation = 1. / (dist + dist*dist);
    
    vec3 cameraVector = normalize( -vPosition.xyz);
    
    
    
    vec3 reflection = normalize( reflect(-lightDir, normal));
    
    vec3 diffuse = vec3( max( dot( normal, lightDir), 0.0) );
    
    const float shininess_factor = 32.;
    
    vec3 specular = vec3( pow( max( dot( reflection, cameraVector), 0.0), shininess_factor) );
    
    oColor = vec4(attenuation * ( specular + diffuse) + ambient, 1.);
    
    
}
