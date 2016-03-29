#version 150

out vec4 			oColor;
in vec4				Color;
in vec3				vNormal;

uniform sampler2D uTex0;
void main( void )
{
    vec3 normal = normalize( -vNormal );
    float diffuse = max( dot( normal, vec3( 0, 0, -1 ) ), 0.0 );
    oColor = Color * diffuse * vec4(1.3);
}