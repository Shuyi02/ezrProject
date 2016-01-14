
#version 330 core

//Input
in vec3 vertex_camera;
in vec3 normal_camera;
in vec2 uv;
in vec3 lightPos_camera;

uniform sampler2D mipMap;

// Ouput data
out vec3 fcolor;

void main()
{	
	vec3 diffLightColor = vec3(1.0f,0.0f,0.0f);
	float lightPower = 50.0f;
	
	float distance = length(lightPos_camera - vertex_camera);
	
	//----------------------------------------------------------------diffuse
	vec3 n = normal_camera;
	vec3 light = normalize(lightPos_camera - vertex_camera); //inverse direction of light
	float cosTheta = clamp( dot( n,light ), 0.0, 1.0 );
	
	//----------------------------------------------------------------specular
	vec3 eye = normalize(vec3(0,0,10) - vertex_camera); //inverse direction of the eye
	vec3 r = reflect(-light,n);
	float cosAlpha = clamp( dot( eye,r ), 0.0 ,1.0 );
    
    //------------------------------------------------ mipMap spaß
	
	// uv between [0,1] 
	vec2 fUV;

	//tone values for translate uv.x
	float tone[6]=float[6](0.0, 1.0/6.0, 2.0/6.0, 3.0/6.0, 4.0/6.0, 5.0/6.0);
	
	//level values for
	//x: scale uv.x, y: translate uv.y, z: scale uv.y
	vec3 level [4];
	level[0] = vec3(6.0, 7.0/15.0, 8.0/15.0);
	level[1] = vec3(12.0, 1.0/5.0, 4.0/15.0); 
	level[2] = vec3(24.0, 1.0/15.0, 2.0/15.0);
	level[3] = vec3(48, 0.0, 1.0/15.0); 

	//----------------------------------------choose tone and level
	int l=0;
	int t=2; 
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 

	fcolor = texture (mipMap, fUV).rgb;


	//--------------------------------------------------------------- six way blend
	//float hatchBrightness = min(1.0, cosTheta+cosAlpha) * 6.0;
	float hatchBrightness = min(1.0,cosTheta) * 5.0;
	
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
	t=0;
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 
	vec4 h0 = texture(mipMap,fUV) * weight0;
	
	t=1;
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 
	vec4 h1 = texture(mipMap,fUV) * weight1;
	
	t=2;
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 
	vec4 h2 = texture(mipMap,fUV) * weight2;
	
	t=3;
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 
	vec4 h3 = texture(mipMap,fUV) * weight3;
	
	t=4;
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 	
	vec4 h4 = texture(mipMap,fUV) * weight4;
	//vec4 h4 = texture(mipMap,fUV);
	
	//t=5;
	//fUV.x = (uv.x/level[l].x)+tone[t];
	//fUV.y = (uv.y*level[l].z)+level[l].y; 
	//vec4 h5 = texture(mipMap,fUV) * weight5;
	
	
	//----------------------------------------------------------------- fragment color
	//fColor = diffLightColor * lightPower * cosTheta / (distance*distance) +
	//diffLightColor * lightPower * pow(cosAlpha,3) / (distance*distance);
	//fcolor = vec3(dot(normalize(normal_camera), normalize(lightPos_camera)));
	fcolor = (white + h1 + h2 + h3 + h4).xyz;
}