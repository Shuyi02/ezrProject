
#version 330 core

//Input
in vec3 vertex_camera;
in vec3 normal_camera;
in vec3 lightPos_camera;
in vec2 uv;
in vec3 shadowCoord;

uniform sampler2D hatch00;
uniform sampler2D hatch01;
uniform sampler2D hatch02;
uniform sampler2D hatch03;
uniform sampler2D hatch04;
uniform sampler2D hatch05;
uniform sampler2D shadowMap;

// Ouput data
out vec3 fcolor;

void main()
{	
	//vec3 diffLightColor = vec3(1.0f,0.0f,0.0f);
	//float lightPower = 50.0f;
	
	//float distance = length(lightPos_camera - vertex_camera);
	
	//----------------------------------------------------------------diffuse
	//vec3 n = normalize(normal_camera);
	//vec3 light = normalize(lightPos_camera - vertex_camera); //inverse direction of light
	//float cosTheta = clamp( dot( n,light ), 0.0, 1.0 );
	
	//----------------------------------------------------------------- fragment color
	//fcolor = diffLightColor * lightPower * cosTheta / (distance*distance) +
	//diffLightColor * lightPower * pow(cosAlpha,3) / (distance*distance);
	//fcolor = vec3(dot(normalize(normal_camera), normalize(lightPos_camera)));
	fcolor = vec3(1.0f,0.0f,0.0f);
	
}