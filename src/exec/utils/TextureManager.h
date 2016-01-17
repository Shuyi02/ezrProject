#ifndef SRC_EXEC_UTILS_TEXTUREMANAGER_H__
#define SRC_EXEC_UTILS_TEXTUREMANAGER_H__

#include <GL/glew.h>
#include <string>

namespace utils {
	unsigned char* loadImage(const char* filename, int* imageWidth, int* imageHeight, int* comp);
	GLuint loadTexture(const char* filename);
	GLuint loadMipMapTexture(std::string path, int levels);
}

#endif
