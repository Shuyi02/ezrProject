#version 330 core

//Input
in vec3 normal_vert;
in vec3 vertex_pos;
in vec3 normal;

// Ouput data
out vec4 fcolor;

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
    
    fcolor = a * diffC;
    
    // test output color = red 
	//color = vec3(1,0,0);
    }
