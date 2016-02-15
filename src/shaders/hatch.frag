
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

// Ouput data
out vec3 fcolor;

void main()
{	
	vec3 diffLightColor = vec3(1.0f,0.0f,0.0f);
	float lightPower = 50.0f;
	
	float distance = length(lightPos_camera - vertex_camera);
	
	//----------------------------------------------------------------diffuse
	vec3 n = normalize(normal_camera);
	vec3 light = normalize(lightPos_camera - vertex_camera); //inverse direction of light
	float cosTheta = clamp( dot( n,light ), 0.0, 1.0 );
	
	//----------------------------------------------------------------specular
	vec3 eye = normalize(-vertex_camera); //inverse direction of the eye
	vec3 r = reflect(-light,n);
	float cosAlpha = clamp( dot( eye,r ), 0.0 ,1.0 );

	//---------------------------------------------------------------- shadow visibility
	//calc the screenSpace of light's perspective of fragment
	vec4 miau = depthMVPFrag * vec4(vertex_model,1.);
	vec3 miau2 = (miau/miau.w).xyz;
	vec3 shadowCoord = miau2*0.5+vec3(0.5);
	
	float bias = 0.0005;
	float visibility = 1.0;
	//check if current fragment(the shadowCoord) is behind the shadow
	vec4 value = texture(shadowMap, shadowCoord.xy);
	//float depth = value.x;
	if ( value.x  <  shadowCoord.z-bias){
		visibility = 0.0;
	}
	if (value.x <= 0.0){
		visibility = 1.0;
	}
	//--------------------------------------------------------------- six way blend
	float hatchBrightness = min(1.0,cosTheta) * 7.0;
	if(visibility == 0.0){
		hatchBrightness = 0.0;
	}
	
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
	
	//----------------------------------------------------------------- fragment color
	//fcolor = diffLightColor * lightPower * cosTheta / (distance*distance) +
	//diffLightColor * lightPower * pow(cosAlpha,3) / (distance*distance);
	//fcolor = vec3(dot(normalize(normal_camera), normalize(lightPos_camera)));
	
	vec3 hatchColor = (white + h0 + h1 + h2 + h3 + h4 + h5).xyz;
	
	//threshold
	fcolor = hatchColor;
	if(hatchColor.x < 0.6){
		fcolor = vec3(0.0);
	}else{
		fcolor = vec3(1.0);
	}
	
	//----------------------------------------------------------------dummy outline ^^
	//mittlere mittelpixel ist gl_FragCoord.xy 
	vec2 suv = gl_FragCoord.xy/vec2(1024,768); 
	
	//float depth = vertex_camera.z;
	//float depth = gl_FragCoord.z;
	//float dduTiefe = dFdx(texture(normalDepth,gl_FragCoord.xy).w); 
	//float ddvTiefe = dFdy(texture(normalDepth,gl_FragCoord.xy).w);
	//vec2 gradTiefe = vec2(dduTiefe,ddvTiefe);
	//float rate = length(gradTiefe);
	
	//vec4 outlineWight;
	//if(rate > 0.392){
	//	outlineWight = vec4(0.0,0.0,0.0,1.0);
	//}
	//else{
	//	outlineWight = vec4(1.0,0.0,0.0,1.0);
	//}
	
	
	// look neighbor pixels in 8th-neighborhood
	//  miauA  miauB  miauC
	//  miauD    X    miauE
	//  miauF  miauG  miauH
	vec4 miauA = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,-1.0))/vec2(1024,768));
	vec4 miauB = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,-1.0))/vec2(1024,768));
	vec4 miauC = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,-1.0))/vec2(1024,768));
	vec4 miauD = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,0.0))/vec2(1024,768));
	vec4 miauE = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,0.0))/vec2(1024,768));
	vec4 miauF = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,1.0))/vec2(1024,768));
	vec4 miauG = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,1.0))/vec2(1024,768));
	vec4 miauH = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,1.0))/vec2(1024,768));
	
	//Sobel for the depth
	float Gx_depth = miauA.w+2*miauD.w+miauF.w-miauC.w-2*miauE.w-miauH.w;
	float Gy_depth = miauA.w+2*miauB.w+miauC.w-miauF.w-2*miauG.w-miauH.w;
	
	
	//TODO: NormalMap in Grau! 0.3 * red+ 0.59 * green+ 0.11 * blue
	float Gx_normal = miauA.x*0.3+miauA.y*0.59+miauA.z*0.11+2*miauD.x*0.3+2*miauD.y*0.59+2*miauD.z*0.11 + miauF.x*0.3-miauF.y*0.59-miauF.z*0.11 - miauC.x*0.3-miauC.y*0.11-miauC.z*0.11 - 2*miauE.x*0.3-2*miauE.y*0.59-2*miauE.z*0.11 - miauH.x*0.3-miauH.y*0.59-miauH.z*0.11;
	float Gy_normal = miauA.w+2*miauB.w+miauC.w-miauF.w-2*miauG.w-miauH.w;
	
	
	//float G = length(Gx)+length(Gy);

//	fcolor *= outlineWight.xyz;
	fcolor *= texture(normalDepth,suv).xyz; //Normalmap with Hatching
//	fcolor = texture(normalDepth,suv).xyz;
//	fcolor = vec3(suv,0.0);
	
}