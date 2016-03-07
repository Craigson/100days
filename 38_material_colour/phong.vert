
#version 150

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;

out vec4 vPosition;
out vec3 vNormal;




void main(void)
{
    vNormal = ciNormalMatrix * ciNormal;
    
    vPosition = ciModelView * ciPosition; //THIS GIVES WORLD-SPACE POSITION OF THE FRAGMENT
    
    gl_Position = ciModelViewProjection * ciPosition;
    
}