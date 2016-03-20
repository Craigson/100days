#version 150

in vec3 vNormal;
in vec4 vPosition;

out vec4 oColor;

void main(void)
{
    vec3 normal = normalize(vNormal);
    oColor = vec4(vec3(1.), 1.0);
}