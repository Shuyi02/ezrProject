#ifndef SRC_EXEC_UTILS_TEXTUREMANAGER_H__
#define SRC_EXEC_UTILS_TEXTUREMANAGER_H__

#include <GL/glew.h>

namespace utils {
	unsigned char* loadImage(int* imageWidth, int* imageHeight, int* comp);
	GLuint loadTexture(int* imageWidth, int* imageHeight);
}

#endif
