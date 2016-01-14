
#version 330 core

//Input
in vec3 vertex_pos;
in vec3 normal;
in vec2 uv;

uniform sampler2D textureSampler;

// Ouput data
out vec3 fcolor;

void main()
{

	//Shuyi
	//fcolor = vec3(dot(normalize(normal), normalize(vec3(1., 2., 1.))));

	// test to show normals
	//fcolor = normalize(normal);
	
	//colors
	vec4 diffC;
	diffC = vec4(1.0,0.0,0.0,1.0);
	vec4 specC;
	specC = vec4(1.0,1.0,0.0,1.0);


	//position of light
	vec3 lightPos = vec3(0.0f, 0.0f, 1.0f);
	
	//vector from surface to light 
    vec3 lightVec = normalize(lightPos - vertex_pos.xyz);
    
    //----------------------------- diffuse 
    //angle between normal and light direction, if negativ then 0
    float cos_phi = max( dot( normalize(normal.xyz), lightVec), 0.0f);
    
    //----------------------------- specular IS BUGGY
    //TODO eyePos sollte uebergeben werden?
    // eyePos is of camera
    //vec3 eyePos = vec3(0.0f,0.0f,5.0f);
    // towards the camera
	//vec3 eye = normalize(eyePos - vertex_pos.xyz);
	// direction in which the triangle reflects the light: reflect(-lightVec*n)
	//vec3 reflection = normalize( reflect( -lightVec, normalize(normal)));
	
	//float cos_psi_n = pow( max( dot( reflection, eye), 0.0f), 50);
    
    float a = cos_phi;
    //float b = cos_psi_n;
    //fcolor = a * diffC + b * specC;
    
    //----------------------------->>>> choose output color
    // test output color = red 
	//fcolor = vec3(1,0,0);
	
    // lighting color
    //vec4 color = a * diffC;
    //fcolor= fcolor.xyz;
	
	// texture color = color of the texture at the specified UV
	
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

	//----------------------------->>>>choose tone and level
	int l=3;
	int t=2; 
	fUV.x = (uv.x/level[l].x)+tone[t];
	fUV.y = (uv.y*level[l].z)+level[l].y; 

	fcolor = texture (textureSampler, fUV).rgb;
	
    }