#version 330 core

// Ouput data
out vec4 fcolor;
in vec4 pos;
in vec4 nor;
uniform mat4 mv;

void main()
{

	// Output color = red 
	//color = vec3(1,0,0);
	//fcolor = normalize(nor);
	
	//colors
	vec4 diffC;
	diffC = vec4(1.0,0.0,0.0,1.0);
	vec4 specC;
	specC = vec4(1.0,1.0,0.0,1.0);


	//lightPos is the position of light
	vec3 lightPos = vec3(0.0f, 0.0f, 1.0f);
	//lightVector is the unit vector that goes from the surface to the light 
    vec3 lightVector = normalize(lightPos - pos.xyz);
    
    //diffuse 
    // cos_phi: angle between the normal and the light direction: (n*lightVector) or if negativ 0
    float cos_phi = max( dot( normalize(nor.xyz), lightVector), 0.0f);
    
    //specular IS BUGGY
    //TODO eyePos sollte uebergeben werden?
    // eyePos is of camera
    vec3 eyePos = vec3(0.0f,0.0f,5.0f);
    // Eye vector (towards the camera)
	vec3 eye = normalize(eyePos - pos.xyz);
	// Direction in which the triangle reflects the light: reflect(-lightVector*n)
	vec3 reflection = normalize( reflect( -lightVector, normalize(nor.xyz)));
	
	float cos_psi_n = pow( max( dot( reflection, eye), 0.0f), 50);
	
  
    
    float a = cos_phi;
    float b = cos_psi_n;
    fcolor = a * diffC + b * specC;
    
    //fcolor = a * diffC;
    }


