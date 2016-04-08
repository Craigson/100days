#version 150

uniform samplerCube uCubeMapTex;  // CUBEMAP TEXTURE SAMPLER
uniform vec3 uCamPos; //camera's position

in vec3 vNormal;
in vec4 vPosition;
in vec4 vColor;

out vec4 oColor;

void main()
{
    //WE SAMPLE FROM THE CUBEMAP TEXTURES USING THE DIRECTION VECTOR
  //  oColor = texture(uCubeMapTex, vDir);
    
    vec3 I = normalize(vPosition.xyz - uCamPos); //EYE DIR VECTOR
    vec3 R = reflect(I, normalize(vNormal)); //REFLECTION VECTOR
    oColor = texture(uCubeMapTex, R);
}