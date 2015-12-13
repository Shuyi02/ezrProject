#include "../utils/stb_image.h"
#include "../utils/TextureManager.h"

#include <iostream>
using namespace std;

namespace utils {
unsigned char* loadImage(int* imageWidth, int* imageHeight, int* comp) {
	int req_comp=0;

	FILE *file = fopen("F:/Uni/EZR/resources/kitty.jpg", "rb");
	if (!file) {
		cout << "can not open image file" << endl;
		return 0;
	}

	unsigned char* image = stbi_load_from_file(file, imageWidth, imageHeight,
			comp, req_comp);

	cout << "width/height image:" << *imageWidth << " " << *imageHeight << endl;

	return image;
}

GLuint loadTexture(int* imageWidth, int* imageHeight) {

	GLuint texture;
	int comp=0;

	unsigned char * data = loadImage(imageWidth, imageHeight, &comp);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, (comp==3)?GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, data);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 5);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap?GL_REPEAT:GL_CLAMP);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap?GL_REPEAT:GL_CLAMP);
	// glBindTexture(GL_TEXTURE_2D,GL_NONE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *imageWidth, *imageHeight, 0,
			(comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

	if (data) stbi_image_free(data);

	return texture;
}
}

