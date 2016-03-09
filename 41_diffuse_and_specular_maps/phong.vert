
#version 150

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;
in vec2 ciTexCoord0;

out vec4 vPosition;
out vec3 vNormal;
out vec2 vTexCoord0;




void main(void)
{
    vNormal = ciNormalMatrix * ciNormal;
    
    vPosition = ciModelView * ciPosition; //THIS GIVES WORLD-SPACE POSITION OF THE FRAGMENT
    
    vTexCoord0 = ciTexCoord0;
    
    gl_Position = ciModelViewProjection * ciPosition;
    
}