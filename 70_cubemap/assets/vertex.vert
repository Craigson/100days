#version 150

uniform mat4 ciModelViewProjection;
uniform mat4 ciModelView;
uniform mat3 ciNormalMatrix;

in vec4 ciPosition;
in vec3 ciNormal;

out vec3 vDir;
out vec3 vNormal;

void main( void )
{
    
    vNormal = ciNormalMatrix * ciNormal;
    
    //THE DIRECTION VECTOR SHOULD BE EQUAL TO THE POSITION VECTOR - ORIGIN (IE VEC3(0).  SO WE SHOULD BE ABLE TO PASS THE VERTEX POSITION AS THE DIRECTION VECTOR;
    //HOWEVER, VERY NB!!!! WE NEED TO PASS THE OBJECT SPACE POSITION, NOT THE VIEW SPACE POSITION, SO WE DON'T MULTIPLY ciPosition by the ciModelView MATRIX;
    
    vDir = normalize(ciPosition.xyz);
    
    gl_Position = ciModelViewProjection * ciPosition;
}