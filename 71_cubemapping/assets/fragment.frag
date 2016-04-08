#version 150

uniform samplerCube uCubeMapTex;  // CUBEMAP TEXTURE SAMPLER
in vec3 vDir;
in vec3 vNormal;

out vec4 oColor;

void main()
{
    //WE SAMPLE FROM THE CUBEMAP TEXTURES USING THE DIRECTION VECTOR
    oColor = texture(uCubeMapTex, vDir);
}