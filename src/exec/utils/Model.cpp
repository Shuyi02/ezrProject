/*
 * ModelLoader.cpp
 *
 *  Created on: Dec 11, 2015
 *      Author: Shuyi
 */

#include "Model.h"

#include <iostream>
#include <stdio.h>

namespace utils {

/**
*	Constructor, loading the specified aiMesh
**/
Model::MeshEntry::MeshEntry(aiMesh *mesh) {

	//	Vertices = vbo[0]
	//	TexCoord = vbo[1]
	//	Normals = vbo[2]
	//	Indices = vbo[3]

	_vbo[0] = NULL;
//	_vbo[1] = NULL;
//	_vbo[2] = NULL;
//	_vbo[3] = NULL;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_elementCount = mesh->mNumFaces * 3;

	if(mesh->HasPositions()) {
		float *vertices = new float[mesh->mNumVertices * 3];
		for(int i = 0; i < mesh->mNumVertices; ++i) {
			vertices[i * 3] = mesh->mVertices[i].x;
			vertices[i * 3 + 1] = mesh->mVertices[i].y;
			vertices[i * 3 + 2] = mesh->mVertices[i].z;
		}

		glGenBuffers(1, &_vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);

		delete[] vertices;
	}


	if(mesh->HasTextureCoords(0)) {
		float *texCoords = new float[mesh->mNumVertices * 2];
		for(int i = 0; i < mesh->mNumVertices; ++i) {
			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
		}

		glGenBuffers(1, &_vbo[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);

		delete[] texCoords;
	}


	if(mesh->HasNormals()) {
		float *normals = new float[mesh->mNumVertices * 3];
		for(int i = 0; i < mesh->mNumVertices; ++i) {
			normals[i * 3] = mesh->mNormals[i].x;
			normals[i * 3 + 1] = mesh->mNormals[i].y;
			normals[i * 3 + 2] = mesh->mNormals[i].z;
		}

		glGenBuffers(1, &_vbo[2]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);

		delete[] normals;
	}


	if(mesh->HasFaces()) {
		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
		for(int i = 0; i < mesh->mNumFaces; ++i) {
			indices[i * 3] = mesh->mFaces[i].mIndices[0];
			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		glGenBuffers(1, &_vbo[3]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (3);

		delete[] indices;
	}


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Model::MeshEntry::~MeshEntry() {
	if(_vbo[0]) {
		glDeleteBuffers(1, &_vbo[0]);
	}

	if(_vbo[1]) {
		glDeleteBuffers(1, &_vbo[1]);
	}

	if(_vbo[2]) {
		glDeleteBuffers(1, &_vbo[2]);
	}

	if(_vbo[3]) {
		glDeleteBuffers(1, &_vbo[3]);
	}

	glDeleteVertexArrays(1, &_vao);
}


void Model::MeshEntry::render() {
	glBindVertexArray(_vao);

	glDrawElements(GL_TRIANGLES, _elementCount, GL_UNSIGNED_INT, NULL);

	glBindVertexArray(0);
}

//-----------------------------------------------------------------

Model::Model(const char *filename) {
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);
			if (!scene) {
				printf("Unable to load mesh: %s\n", importer.GetErrorString());
			}

			for (int i = 0; i < scene->mNumMeshes; ++i) {
				_meshEntries.push_back(new Model::MeshEntry(scene->mMeshes[i]));
			}
		}

Model::~Model() {

	for(int i = 0; i < _meshEntries.size(); ++i) {
		delete _meshEntries.at(i);
	}
	_meshEntries.clear();
}

void Model::render() {
	for(int i = 0; i < _meshEntries.size(); ++i) {
		_meshEntries.at(i)->render();
	}
}

} /* namespace utils */
