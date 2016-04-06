#version 150

in vec2 TexCoords;
out vec4 oColour;
uniform sampler2D screenTexture;

void main(void)
{
    oColour = texture(screenTexture, TexCoords);
    float average = 0.2126 * oColour.r + 0.7152 * oColour.g + 0.0722 * oColour.b;
    oColour = vec4(average, average, average, 1.0);
}