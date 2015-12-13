#version 330 core
// Input data
// interpolated texture values from the vertex shaders
in vec2 UV;

// Ouput data
out vec3 color;

// Constant data
uniform sampler2D textureSampler;

void main()
{

	// Output color = color of the texture at the specified UV 
	color = texture (textureSampler, UV).rgb;

}