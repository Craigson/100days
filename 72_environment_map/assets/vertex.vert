#version 150

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;
uniform mat4 ciViewMatrix;

in vec4 ciPosition;
in vec4 ciColor;
in vec3 ciNormal;


out vec4 vPosition;
out vec4 vColor;
out vec3 vNormal;

void main( void )
{
    vNormal = normalize(ciNormalMatrix * ciNormal);
    vPosition = ciModelView * ciPosition;
    gl_Position = ciModelViewProjection * ciPosition;
}