
#version 150

uniform mat4 ciModelViewProjection;
in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 TexCoords;

void main(void)
{
    TexCoords = ciTexCoord0;
    gl_Position = ciModelViewProjection * ciPosition;
    //gl_Position = vec4(ciPosition.x, ciPosition.y, 0.f, 1.f);
}