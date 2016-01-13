
#version 330 core

//Input
in vec3 vertex_pos;
in vec3 normal;
in vec2 uv;

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
	//colors
	vec4 diffC;
	diffC = vec4(1.0,1.0,1.0,1.0);
	vec4 specC;
	specC = vec4(1.0,1.0,0.0,1.0);

	//position of light
	vec3 lightPos = vec3(-1.0f, 1.0f, 0.0f);
	
	//vector from surface to light 
    vec3 lightVec = normalize(lightPos - vertex_pos.xyz);
    
    //---------------------------------------------- diffuse angle
    //angle between normal and light direction, if negativ then 0
    float diff = max( dot( normalize(normal.xyz), lightVec), 0.0f);
    
    //---------------------------------------------- specular IS BUGGY
    //TODO eyePos sollte uebergeben werden?
    // eyePos is of camera
    //vec3 eyePos = vec3(0.0f,0.0f,5.0f);
    // towards the camera
	//vec3 eye = normalize(eyePos - vertex_pos.xyz);
	// direction in which the triangle reflects the light: reflect(-lightVec*n)
	//vec3 reflection = normalize( reflect( -lightVec, normalize(normal)));
	//float spec = pow( max( dot( reflection, eye), 0.0f), 50);
    //color_light = diff * diffC + cos_psi_n * specC;
    
    //---------------------------------------------------- light color
    vec4 diffLight = diffC * diff;
    //vec4 specLight = specC * spec
    vec4 lightColor = diffLight;
    //fcolor= fcolor.xyz;
	
	//----------------------------------------------------- texture color
	vec4 texColor = texture(hatch00, uv);
	
	float hatchLevel = min(1.0, diff) * 6.0;
	float auf = floor(hatchLevel);
	float ab = ceil(hatchLevel);
	
	//----------------------------------------------------- six way blend
	float weight0, weight1, weight2, weight3, weight4, weight5, weightWhite = 0.0;
	
	if(hatchLevel > 5.0){
		weightWhite = 1.0 - (6.0-hatchLevel);
		weight0 = 1.0 - weightWhite;
	}else if(hatchLevel > 4.0){
		weight0 = 1.0 - (5.0 - hatchLevel);
        weight1 = 1.0 - weight0;
	}else if(hatchLevel > 3.0) {
        weight1 = 1.0 - (4.0 - hatchLevel);
        weight2 = 1.0 - weight1;
    }else if(hatchLevel > 2.0) {
        weight2 = 1.0 - (3.0 - hatchLevel);
        weight3 = 1.0 - weight2;
    } else if(hatchLevel > 1.0) {
        weight3 = 1.0 - (2.0 - hatchLevel);
        weight4 = 1.0 - weight3;
    } else {
        weight4 = 1.0 - (1.0 - hatchLevel);
        weight5 = 1.0 - weight4;
    }
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0) * weightWhite;
	vec4 h0 = texture(hatch00,uv) * weight0;
	vec4 h1 = texture(hatch01,uv) * weight1;
	vec4 h2 = texture(hatch02,uv) * weight2;
	vec4 h3 = texture(hatch03,uv) * weight3;
	vec4 h4 = texture(hatch04,uv) * weight4;
	vec4 h5 = texture(hatch05,uv) * weight5;
	
	//----------------------------------------------------- fragment color
	//fcolor = lightColor.xyz * texColor.rgb;
	fcolor = (white + h0 + h1 + h2 + h3 + h4 + h5).xyz;
    }