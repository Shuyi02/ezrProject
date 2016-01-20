#include "../utils/stb_image.h"
#include "../utils/TextureManager.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace utils {
unsigned char* loadImage(const char* filename, int* imageWidth, int* imageHeight, int* comp) {
	int req_comp=0;

	FILE *file = fopen(filename, "rb");
	if (!file) {
		cerr << "TextureManager.cpp line 11: image file not found" << endl;
		return 0;
	}

	unsigned char* image = stbi_load_from_file(file, imageWidth, imageHeight,
			comp, req_comp);

	return image;
}

GLuint loadTexture(const char* filename) {

	GLuint texture;
	int comp=0;
	int imageWidth=0;
	int imageHeight=0;

	unsigned char * data = loadImage(filename, &imageWidth, &imageHeight, &comp);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			(comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

	if (data) stbi_image_free(data);

	return texture;
}

GLuint loadMipMapTexture(string path, int levels){

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);

	int comp=0;
	int imageWidth=0;
	int imageHeight=0;


	//path = ResourcePATH/hatches/hatch0
	for(int i = 0; i < levels; i++){

		stringstream ss;
		ss<<path<<i<<".bmp";

		string filename = ss.str();
		ss.clear();

		unsigned char * data = loadImage(filename.c_str(), &imageWidth, &imageHeight, &comp);

		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, imageWidth, imageHeight, 0,
				(comp == 4) ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

		if (data) stbi_image_free(data);

	}
	return texture;
}
}

