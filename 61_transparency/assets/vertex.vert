#version 150

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;
in vec4 ciColor;

out vec4 vPosition;
out vec3 vNormal;
out vec2 vTexCoord;
out vec4 vColor;

void main(void)
{
    vTexCoord = ciTexCoord0;
    vNormal = ciNormalMatrix * ciNormal;
    vPosition = ciModelView * ciPosition;
    vColor = ciColor;
    gl_Position = ciModelViewProjection * ciPosition;
    
}