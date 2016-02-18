#version 330 core

//Input
in vec3 vertex_camera;
in vec3 normal_camera;
in vec3 lightPos_camera;
in vec2 uv;
in vec3 vertex_model;
in mat4 depthMVPFrag;

uniform sampler2D hatch00;
uniform sampler2D hatch01;
uniform sampler2D hatch02;
uniform sampler2D hatch03;
uniform sampler2D hatch04;
uniform sampler2D hatch05;
uniform sampler2D shadowMap;
uniform sampler2D normalDepth;

//window 
uniform int windowHeight;
uniform int windowWidth;

// Ouput data
out vec3 fcolor;

void main()
{	
	float distance = length(lightPos_camera - vertex_camera);
	
	//----------------------------------------------------------------diffuse
	vec3 n = normalize(normal_camera);
	vec3 light = normalize(lightPos_camera - vertex_camera); //inverse direction of light
	float cosTheta = clamp( dot( n,light ), 0.0, 1.0 );
	
	//----------------------------------------------------------------specular
	vec3 eye = normalize(-vertex_camera); //inverse direction of the eye
	vec3 r = reflect(-light,n);
	float cosAlpha = clamp( dot( eye,r ), 0.0 ,1.0 );

	//--------------------------------------------------------------- six way blend
	float hatchBrightness = min(1.0,cosTheta) * 7.0;
	
	float weightWhite = 0.0;
	float weight0 = 0.0;
	float weight1 = 0.0;
	float weight2 = 0.0;
	float weight3 = 0.0;
	float weight4 = 0.0;
	float weight5 = 0.0;
	
	if(hatchBrightness > 5.0){
		weightWhite = 1.0;
	}else if(hatchBrightness > 3.0){
		weightWhite = 1.0 - (3.0-hatchBrightness/2.0);
		weight0 = 1.0 - weightWhite;
	}else if(hatchBrightness > 2.5){
		weight0 = 1.0 - ((3.0 - hatchBrightness)/0.5);
        weight1 = 1.0 - weight0;
	}else if(hatchBrightness > 2.0) {
        weight1 = 1.0 - ((2.5 - hatchBrightness)/0.5);
        weight2 = 1.0 - weight1;
    }else if(hatchBrightness > 1.5) {
        weight2 = 1.0 - ((2.0 - hatchBrightness)/0.5);
        weight3 = 1.0 - weight2;
    } else if(hatchBrightness > 1.0) {
        weight3 = 1.0 - ((1.5 - hatchBrightness)/0.5);
        weight4 = 1.0 - weight3;
    } else {
        weight4 = 1.0 - ((1.0 - hatchBrightness)/0.5);
        weight5 = 1.0 - weight4;
    }
    
    //------------------------------------------------- blend hatch
	
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0) * weightWhite;
	vec4 h0 = texture(hatch00,uv) * weight0;
	vec4 h1 = texture(hatch01,uv) * weight1;
	vec4 h2 = texture(hatch02,uv) * weight2;
	vec4 h3 = texture(hatch03,uv) * weight3;
	vec4 h4 = texture(hatch04,uv) * weight4;
	vec4 h5 = texture(hatch05,uv) * weight5;
	
	//----------------------------------------------------------------- hatch color
	vec3 hatchColor = (white + h0 + h1 + h2 + h3 + h4 + h5).xyz;
	
	//threshold
	if(hatchColor.x < 0.6){
		hatchColor = vec3(0.0);
	}else{
		hatchColor = vec3(1.0);
	}


	//------------------------------------------------------------------- outline

	// look neighbor pixels in 8th-neighborhood
	vec4 miauX = texture(normalDepth, gl_FragCoord.xy/vec2(windowWidth,windowHeight));
	vec4 miauA = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,-1.0))/vec2(windowWidth,windowHeight));
	vec4 miauB = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,-1.0))/vec2(windowWidth,windowHeight));
	vec4 miauC = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,-1.0))/vec2(windowWidth,windowHeight));
	vec4 miauD = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,0.0))/vec2(windowWidth,windowHeight));
	vec4 miauE = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,0.0))/vec2(windowWidth,windowHeight));
	vec4 miauF = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,1.0))/vec2(windowWidth,windowHeight));
	vec4 miauG = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,1.0))/vec2(windowWidth,windowHeight));
	vec4 miauH = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,1.0))/vec2(windowWidth,windowHeight));
	
	//gradients in x and y
	vec4 Gx = miauA + 2*miauD + miauF - miauC - 2*miauE - miauH;
	vec4 Gy = miauA + 2*miauB + miauC - miauF - 2*miauG - miauH;
	
	//gradients components
	float gDepth = sqrt(Gx.w*Gx.w + Gy.w*Gy.w);
	float gNormalx = sqrt(Gx.x*Gx.x + Gy.x*Gy.x);
	float gNormaly = sqrt(Gx.y*Gx.y + Gy.y*Gy.y);
	float gNormalz= sqrt(Gx.z*Gx.z + Gy.z*Gy.z);
	
	//---------------------------------------------------------------------------frag color
	fcolor = hatchColor;
	
	//threshold for outline (or something different, maybe with paper normals, dunno)
	if(gNormaly >= 0.7 || gNormalx >= 0.7 || gNormalz >= 0.7 || gDepth >= 0.005){
		fcolor = vec3(0.0, 0.0, 0.0);
	}

}