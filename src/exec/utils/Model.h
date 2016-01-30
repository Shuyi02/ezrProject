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
#include <glm/glm.hpp>

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
		GLuint _vbo[5];

		unsigned int _elementCount;
		void calcTexCoord(glm::vec3 textureDir, glm::vec3 triangleA,
				glm::vec3 triangleB, glm::vec3 triangleC, glm::vec2& u1,
				glm::vec2& u2, glm::vec2& u3);

		void getCurvatureTensor(glm::vec3 triangleA,
				glm::vec3 triangleB, glm::vec3 triangleC, glm::vec3 normalA,
				glm::vec3 normalB, glm::vec3 normalC, glm::vec3& curvatureDirection);
	};
	Model(const char *filename);
	virtual ~Model();

	void render();

	std::vector<MeshEntry*> _meshEntries;
};

} /* namespace utils */

#endif /* SRC_EXEC_UTILS_MODEL_H_ */
