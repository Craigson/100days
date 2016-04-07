#version 150

uniform samplerCube uCubeMap;  // CUBEMAP TEXTURE SAMPLER
in vec3 vDir;
in vec3 vNormal;

out vec4 oColor;

void main()
{
    //DO SOME SIMPLE LIGHTING
    vec3 normal = normalize( -vNormal );
    float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0 );
    
    //WE SAMPLE FROM THE CUBEMAP TEXTURES USING THE DIRECTION VECTOR
    oColor = texture(uCubeMap, vDir) * diffuse;
}