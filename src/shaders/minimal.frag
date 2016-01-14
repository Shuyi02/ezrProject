
#version 330 core

//Input
in vec3 vertex_camera;
in vec3 normal_camera;
in vec2 uv;
in vec3 lightPos_camera;

uniform sampler2D hatch00;
uniform sampler2D hatch01;
uniform sampler2D hatch02;
uniform sampler2D hatch03;
uniform sampler2D hatch04;
uniform sampler2D hatch05;

// Ouput data
out vec3 fcolor;

void main()
{	
	vec3 diffLightColor = vec3(1.0f,0.0f,0.0f);
	float lightPower = 50.0f;
	
	float distance = length(lightPos_camera - vertex_camera);
	
	//----------------------------------------------------------------diffuse
	vec3 n = normal_camera;
	vec3 l = normalize(lightPos_camera - vertex_camera); //inverse direction of light
	float cosTheta = clamp( dot( n,l ), 0.0, 1.0 );
	
	//----------------------------------------------------------------specular
	vec3 eye = normalize(vec3(0,0,10) - vertex_camera); //inverse direction of the eye
	vec3 r = reflect(-l,n);
	float cosAlpha = clamp( dot( eye,r ), 0.0 ,1.0 );
	
	//--------------------------------------------------------------- six way blend
	//float hatchBrightness = min(1.0, cosTheta+cosAlpha) * 6.0;
	float hatchBrightness = min(1.0, cosTheta) * 6.0;
	
	float weight0, weight1, weight2, weight3, weight4, weight5, weightWhite = 0.0;
	
	if(hatchBrightness > 5.0){
		weightWhite = 1.0 - (6.0-hatchBrightness);
		weight0 = 1.0 - weightWhite;
	}else if(hatchBrightness > 4.0){
		weight0 = 1.0 - (5.0 - hatchBrightness);
        weight1 = 1.0 - weight0;
	}else if(hatchBrightness > 3.0) {
        weight1 = 1.0 - (4.0 - hatchBrightness);
        weight2 = 1.0 - weight1;
    }else if(hatchBrightness > 2.0) {
        weight2 = 1.0 - (3.0 - hatchBrightness);
        weight3 = 1.0 - weight2;
    } else if(hatchBrightness > 1.0) {
        weight3 = 1.0 - (2.0 - hatchBrightness);
        weight4 = 1.0 - weight3;
    } else {
        weight4 = 1.0 - (1.0 - hatchBrightness);
        weight5 = 1.0 - weight4;
    }
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0) * weightWhite;
	vec4 h0 = texture(hatch00,uv) * weight0;
	vec4 h1 = texture(hatch01,uv) * weight1;
	vec4 h2 = texture(hatch02,uv) * weight2;
	vec4 h3 = texture(hatch03,uv) * weight3;
	vec4 h4 = texture(hatch04,uv) * weight4;
	vec4 h5 = texture(hatch05,uv) * weight5;
	
	//----------------------------------------------------------------- fragment color
	//fColor = diffLightColor * lightPower * cosTheta / (distance*distance) +
	//diffLightColor * lightPower * pow(cosAlpha,3) / (distance*distance);
	//fcolor = vec3(dot(normalize(normal_camera), normalize(lightPos_camera)));
	fcolor = (white + h0 + h1 + h2 + h3 + h4 + h5).xyz;
}