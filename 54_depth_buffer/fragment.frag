#version 150

out vec4 oColour;

//THESE VALUES SHOULD BE THE SAME AS WHAT ARE SET FOR THE CLIPPING PLANES ON THE CPU-SIDE
float near = 1.0;
float far = 100.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; //CONVERT BACK TO NORMALIZED DEVICE COORDINATES
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void)
{
    float depth = LinearizeDepth(gl_FragCoord.z) / far; //DIVIDE BY FAR FOR DEMONSTRATION
    oColour = vec4(vec3(depth), 1.f);
//    oColour = vec4(1.f);
}
