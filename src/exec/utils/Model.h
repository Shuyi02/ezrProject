/*
 * ModelLoader.h
 *
 *  Created on: Dec 11, 2015
 *      Author: Shuyi
 */

#ifndef SRC_EXEC_UTILS_MODEL_H_
#define SRC_EXEC_UTILS_MODEL_H_


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <vector>

namespace utils {

class Model {
public:
	struct MeshEntry {
		MeshEntry(aiMesh *mesh);
		~MeshEntry();

		void load(aiMesh *mesh);
		void render();

		GLuint _vao;
		GLuint _vbo[4];

		unsigned int _elementCount;
	};

public:
	Model(const char *filename);
	virtual ~Model();

	void render();

	std::vector<MeshEntry*> _meshEntries;
};

} /* namespace utils */

#endif /* SRC_EXEC_UTILS_MODEL_H_ */
