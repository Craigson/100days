#version 150

in vec2 TexCoords;
out vec4 oColour;
uniform sampler2D screenTexture;

void main(void)
{
    oColour = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
}