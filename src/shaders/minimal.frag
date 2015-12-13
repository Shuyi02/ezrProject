#version 330 core

//Input
in vec3 normal_vert;

// Ouput data
out vec3 color;

void main()
{

	// Output color = red 
	//color = vec3(1,0,0);
	color = vec3(dot(normalize(normal_vert), normalize(vec3(1., 2., 1.))));

}