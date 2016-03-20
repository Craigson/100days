#version 150

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec3 ciNormal;

out vec4 vPosition;
out vec4 Color;
out vec3 vNormal;

void main(void)
{
    
    vNormal = ciNormalMatrix * ciNormal;
    
    vPosition = ciModelView * ciPosition;
    
    gl_Position = ciModelViewProjection * ciPosition;
    
}