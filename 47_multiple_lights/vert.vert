#version 150

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;

in vec4 ciPosition;
in vec3 ciNormal;

out vec3 vNormal;
out vec4 vPosition;

void main(void)
{
    vNormal = normalize(ciNormalMatrix * ciNormal);
    
    vPosition = ciModelView * ciPosition;
    
    gl_Position = ciModelViewProjection * ciPosition;
}