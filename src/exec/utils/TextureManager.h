#ifndef SRC_EXEC_UTILS_TEXTUREMANAGER_H__
#define SRC_EXEC_UTILS_TEXTUREMANAGER_H__

#include <GL/glew.h>

namespace utils {
	unsigned char* loadImage(const char* filename, int* imageWidth, int* imageHeight, int* comp);
	GLuint loadTexture(const char* filename, int* imageWidth, int* imageHeight);
}

#endif
