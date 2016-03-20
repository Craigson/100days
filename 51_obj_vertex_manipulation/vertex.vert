#version 150

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;
uniform mat4 ciModelViewProjection;

uniform float uTime;

in vec4 ciPosition;
in vec3 ciNormal;

out vec4 vPosition;
out vec4 Color;
out vec3 vNormal;

void main(void)
{
    
    vNormal = normalize(ciNormalMatrix * ciNormal);
    
    float time = uTime;
    
    float displacement = cos(uTime);
    
    vPosition = ciModelView * ciPosition;
    
    vec3 newPosition = vec3(ciPosition.xyz + vNormal * displacement);
    
    gl_Position = ciModelViewProjection * vec4(newPosition, 1.);
//    gl_Position = ciModelViewProjection * ciPosition;
    
}