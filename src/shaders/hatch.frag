
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
	vec4 miauX = texture(normalDepth, gl_FragCoord.xy/vec2(1024,768));
	vec4 miauA = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,-1.0))/vec2(1024,768));
	vec4 miauB = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,-1.0))/vec2(1024,768));
	vec4 miauC = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,-1.0))/vec2(1024,768));
	vec4 miauD = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,0.0))/vec2(1024,768));
	vec4 miauE = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,0.0))/vec2(1024,768));
	vec4 miauF = texture(normalDepth,(gl_FragCoord.xy + vec2(-1.0,1.0))/vec2(1024,768));
	vec4 miauG = texture(normalDepth,(gl_FragCoord.xy + vec2(0.0,1.0))/vec2(1024,768));
	vec4 miauH = texture(normalDepth,(gl_FragCoord.xy + vec2(1.0,1.0))/vec2(1024,768));
	
	//gradients in x and y
	vec4 Gx = miauA + 2*miauD + miauF - miauC - 2*miauE - miauH;
	vec4 Gy = miauA + 2*miauB + miauC - miauF - 2*miauG - miauH;
	
	//gradients components
	float gDepth = sqrt(Gx.w*Gx.w + Gy.w*Gy.w);
	float gNormalx = sqrt(Gx.x*Gx.x + Gy.x*Gy.x);
	float gNormaly = sqrt(Gx.y*Gx.y + Gy.y*Gy.y);
	float gNormalz= sqrt(Gx.z*Gx.z + Gy.z*Gy.z);
	
	//all in all gradient, should be used differently maybe :D
	float g = gDepth + gNormalx + gNormaly + gNormalz;
	
	
	//--------------------------------- test outline more paperlike --------------------------------------------------
	// ich war mir nicht sicher ob wenn man den gesamten vektor als gradi nimmt das gleiche raus kommt, wie wenn man davor daraus nur einen "grauwert" macht
	//test outline more paperlike -> NormalMap Gray 
	float mauA = miauA.x*0.3 + miauA.y*0.59 + miauA.z*0.11;
	float mauB = miauB.x*0.3 + miauB.y*0.59 + miauB.z*0.11;
	float mauC = miauC.x*0.3 + miauC.y*0.59 + miauC.z*0.11;
	float mauD = miauD.x*0.3 + miauD.y*0.59 + miauD.z*0.11;
	float mauE = miauE.x*0.3 + miauE.y*0.59 + miauE.z*0.11;
	float mauF = miauF.x*0.3 + miauF.y*0.59 + miauF.z*0.11;
	float mauG = miauG.x*0.3 + miauG.y*0.59 + miauG.z*0.11;
	float mauH = miauH.x*0.3 + miauH.y*0.59 + miauH.z*0.11;

	float mA = abs(miauA.x-miauX.x)*0.3 + abs(miauA.y-miauX.y)*0.59 + abs(miauA.z-miauX.z)*0.11;
	float mB = abs(miauB.x-miauX.x)*0.3 + abs(miauB.y-miauX.y)*0.59 + abs(miauB.z-miauX.z)*0.11;
	float mC = abs(miauC.x-miauX.x)*0.3 + abs(miauC.y-miauX.y)*0.59 + abs(miauC.z-miauX.z)*0.11;
	float mD = abs(miauD.x-miauX.x)*0.3 + abs(miauD.y-miauX.y)*0.59 + abs(miauD.z-miauX.z)*0.11;
	float mE = abs(miauE.x-miauX.x)*0.3 + abs(miauE.y-miauX.y)*0.59 + abs(miauE.z-miauX.z)*0.11;
	float mF = abs(miauF.x-miauX.x)*0.3 + abs(miauF.y-miauX.y)*0.59 + abs(miauF.z-miauX.z)*0.11;
	float mG = abs(miauG.x-miauX.x)*0.3 + abs(miauG.y-miauX.y)*0.59 + abs(miauG.z-miauX.z)*0.11;
	float mH = abs(miauH.x-miauX.x)*0.3 + abs(miauH.y-miauX.y)*0.59 + abs(miauH.z-miauX.z)*0.11;
	
	//float g = 0.125*(length(miauA-miauX) + 2*(length(miauB-miauX)) + length(miauC-miauX) + 2*(length(miauD-miauX)) + 2*(length(miauE-miauX)) + length(miauF-miauX) + 2*(length(miauG-miauX)) + length(miauH-miauX));
	float g_depth = 0.75*(abs(miauA.w-miauX.w) + 2*(abs(miauB.w-miauX.w)) + abs(miauC.w-miauX.w) + 2*(abs(miauD.w-miauX.w)) + 2*(abs(miauE.w-miauX.w)) + abs(miauF.w-miauX.w) + 2*(abs(miauG.w-miauX.w)) + abs(miauH.w-miauX.w));
	float g_normal = 0.75*(mA + 2*mB + mC + 2*mD + 2*mE + mF + 2*mG + mH);
	
	//gradient normal
	float Gx_normal = mauA + 2*mauD + mauF - mauC - 2*mauE - mauH;
	float Gy_normal = mauA + 2*mauB + mauC - mauF - 2*mauG - mauH;
	
	//gradient depth
	float Gx_depth = miauA.w + 2*miauD.w + miauF.w - miauC.w - 2*miauE.w - miauH.w;
	float Gy_depth = miauA.w + 2*miauB.w + miauC.w - miauF.w - 2*miauG.w - miauH.w;
	
	//gradients components
	float gDepth_2 = sqrt(Gx_depth*Gx_depth + Gy_depth*Gy_depth);
	float gNormal_2 = sqrt(Gx_normal*Gx_normal + Gy_normal*Gy_normal);
	
	//---------- optional try more like paper maybe for a better or worse result lol
	//float Gx_normal = length(miauA.xyz-miauX and so on) + 2*length(miauD.xyz) + length(miauF.w) 
	//				  - length(miauC.xyz) - 2*length(miauE.xyz) - length(miauH.xyz);
	//float Gy_normal = length(miauA.xyz) + 2*length(miauB.xyz) + length(miauC.xyz) 
	//				  - length(miauF.xyz) - 2*length(miauG.xyz) - length(miauH.xyz);
	//float gDepth = sqrt(Gx.w*Gx.w + Gy.w*Gy.w);
	//float gNormalx = sqrt(Gx_normal*Gx_normal + Gy_normal*Gy_normal);
	//float gNormaly = sqrt(Gx_normal*Gx_normal + Gy_normal*Gy_normal);
	//float gNormaly = sqrt(Gx_normal*Gx_normal + Gy_normal*Gy_normal);

	
	//---------------------------------------------------------------------------frag color
	fcolor = hatchColor;
	
	//threshold for outline (or something different, maybe with paper normals, dunno)
	//if( gNormaly >= 0.45|| gNormalx >= 0.6 || gNormalz >= 0.6 || gDepth >= 0.005   ){
	//if( gNormal_2 >= ((g_normal*g_normal)/0.04) || gDepth >= 0.005   ){  //fuuuu g_normal blaa wie im paper geht nicht
	if( gNormal_2 >= 0.6 || gDepth >= 0.005 ){
		fcolor = vec3(0.0, 0.0, 0.0);
	}
	
	//if(gDepth_2 <= g_depth || gNormal_2 <= g_normal){ // fuuu paper threshold geht nicht
	//	fcolor = vec3(0.0, 0.0, 0.0);
	//}

}