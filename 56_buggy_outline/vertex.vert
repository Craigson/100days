#version 150

uniform mat4	ciModelViewProjection;
uniform mat3	ciNormalMatrix;

in vec4		ciPosition;
in vec2		ciTexCoord0;
in vec3		ciNormal;
out highp vec2	vTexCoord;
in vec4		ciColor;
out lowp vec4	Color;
out highp vec3	vNormal;

void main( void )
{
    gl_Position	= ciModelViewProjection * ciPosition;
    Color 		= ciColor;
    vTexCoord	= ciTexCoord0;
    vNormal		= ciNormalMatrix * ciNormal;
}