/*
 * ModelLoader.cpp
 *
 *  Created on: Dec 11, 2015
 *      Author: Shuyi
 */

#include "Model.h"

#include <iostream>
#include <stdio.h>
#include <glm/glm.hpp>

namespace utils {


glm::vec3 projectIntoPlane(glm::vec3 globalDirection, glm::vec3 a, glm::vec3 b, glm::vec3 c){
	//calc normal
	glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(b-a), glm::vec3(c-a)));
	//project
	return glm::normalize(globalDirection - (glm::dot(normal,globalDirection)*normal));
}

/**
*	Constructor, loading the specified aiMesh
**/
Model::MeshEntry::MeshEntry(aiMesh *mesh) {

	//	Vertices = vbo[0]
	//	Normals = vbo[1]
	//	TexCoord = vbo[2]
	//	Indices = vbo[3]

	_vbo[0] = NULL;
	_vbo[1] = NULL;
	_vbo[2] = NULL;
	_vbo[3] = NULL;
	_vbo[4] = NULL;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_elementCount = mesh->mNumFaces * 3;

	//------------------------------Test no use of indexbuffer
	//over index
	if(mesh->HasFaces()) {
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		for(int i = 0; i < mesh->mNumFaces; ++i) {
			//push indices for 3 vertices of a triangle
			int first = mesh->mFaces[i].mIndices[0];
			int second = mesh->mFaces[i].mIndices[1];
			int third = mesh->mFaces[i].mIndices[2];

			//each vertex get a vertex, normal and each triangle one uv
			glm::vec3 a(mesh->mVertices[first].x, mesh->mVertices[first].y, mesh->mVertices[first].z);
			glm::vec3 b(mesh->mVertices[second].x, mesh->mVertices[second].y, mesh->mVertices[second].z);
			glm::vec3 c(mesh->mVertices[third].x, mesh->mVertices[third].y, mesh->mVertices[third].z);
			vertices.push_back(a);
			vertices.push_back(b);
			vertices.push_back(c);

			normals.push_back(glm::vec3(mesh->mNormals[first].x, mesh->mNormals[first].y, mesh->mNormals[first].z));
			normals.push_back(glm::vec3(mesh->mNormals[second].x, mesh->mNormals[second].y, mesh->mNormals[second].z));
			normals.push_back(glm::vec3(mesh->mNormals[third].x, mesh->mNormals[third].y, mesh->mNormals[third].z));


			//TODO tangets

			//TODO rotate into same direction as an direction (e.g. global up)
			glm::vec3 globalDir(1.0, 0.0, 0.0);
			glm::vec3 proj = projectIntoPlane(globalDir, a, b, c);

			glm::vec3 firstSide = glm::normalize(glm::vec3(b-a));
			float cosAlph = glm::dot(firstSide,proj);
			float angle = -acos(cosAlph);
			//std::cout<<"miau:  "<<angle<<std::endl;

//			if(angle > 3.1415/2.0) angle = angle-3.1415;


			glm::mat2 rotate = glm::mat2(cos(angle), sin(angle),
					-sin(angle), cos(angle));
//			glm::mat2 rotate = glm::mat2(cos(angle), -sin(angle),
//					sin(angle), cos(angle));

			//Texcoords
			glm::vec2 u1(0.0,0.0);
			glm::vec2 u2(1.0,0.0);
			glm::vec2 u3(0.5,1.0);

			//middle
//			glm::vec2 center = (u1+u2+u3)/3.f;
			glm::vec2 center(0.5,0.5);

//			texCoords.push_back(u1);
//			texCoords.push_back(u2);
//			texCoords.push_back(u3);
//			texCoords.push_back(rotate*u1);
//			texCoords.push_back(rotate*u2);
//			texCoords.push_back(rotate*u3);
			texCoords.push_back(center + (rotate * (u1-center)));
			texCoords.push_back(center + (rotate * (u2-center)));
			texCoords.push_back(center + (rotate * (u3-center)));
		}

		//buffers vertex
		glGenBuffers(1, &_vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);

		//buffer normal
		glGenBuffers(1, &_vbo[1]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (1);

		//buffer texCoord
		glGenBuffers(1, &_vbo[2]);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &texCoords[0], GL_STATIC_DRAW);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);
	}

//----------------------------------------------------------------------------------

	//	if(mesh->HasPositions()) {
	//		float *vertices = new float[mesh->mNumVertices * 3];
	//		for(int i = 0; i < mesh->mNumVertices; ++i) {
	//			vertices[i * 3] = mesh->mVertices[i].x;
	//			vertices[i * 3 + 1] = mesh->mVertices[i].y;
	//			vertices[i * 3 + 2] = mesh->mVertices[i].z;
	//		}
	//
	//		glGenBuffers(1, &_vbo[0]);
	//		glBindBuffer(GL_ARRAY_BUFFER, _vbo[0]);
	//		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//		glEnableVertexAttribArray (0);
	//
	//		delete[] vertices;
	//	}
	//
	//	if(mesh->HasNormals()) {
	//		float *normals = new float[mesh->mNumVertices * 3];
	//		for(int i = 0; i < mesh->mNumVertices; ++i) {
	//			normals[i * 3] = mesh->mNormals[i].x;
	//			normals[i * 3 + 1] = mesh->mNormals[i].y;
	//			normals[i * 3 + 2] = mesh->mNormals[i].z;
	//		}
	//
	//		glGenBuffers(1, &_vbo[1]);
	//		glBindBuffer(GL_ARRAY_BUFFER, _vbo[1]);
	//		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), normals, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//		glEnableVertexAttribArray (1);
	//
	//		delete[] normals;
	//	}
	//
	//	if(mesh->HasTextureCoords(0)) {
	//		//get TextureCoords and normalize Coords between 0 and 1
	//		float *texCoords = new float[mesh->mNumVertices * 2];
	////		float minX=mesh->mTextureCoords[0][0].x;
	////		float maxX=mesh->mTextureCoords[0][0].x;
	////		float minY=mesh->mTextureCoords[0][0].y;
	////		float maxY=mesh->mTextureCoords[0][0].y;
	//		for(int i = 0; i < mesh->mNumVertices; ++i) {
	////			texCoords[i * 2] = mesh->mTextureCoords[0][i].x;
	//			texCoords[i * 2] = (mesh->mTextureCoords[0][i].x)*3;
	////			if (texCoords[i * 2] <minX) minX=texCoords[i * 2] ;
	////			if (texCoords[i * 2] >maxX) maxX=texCoords[i * 2] ;
	//
	////			texCoords[i * 2 + 1] = mesh->mTextureCoords[0][i].y;
	//			texCoords[i * 2 + 1] = (mesh->mTextureCoords[0][i].y)*3;
	////			if (texCoords[i * 2+1] <minY) minY=texCoords[i * 2+1] ;
	////			if (texCoords[i * 2+1] >maxY) maxY=texCoords[i * 2+1] ;
	//		}
	////
	////		float sumX=maxX-minX;
	////		float sumY=maxY-minY;
	////
	////		for(int i = 0; i < mesh->mNumVertices; ++i) {
	////				texCoords[i * 2] = (texCoords[i * 2]-minX)/sumX;
	//////				texCoords[i * 2] = ((texCoords[i * 2]-minX)/sumX)*3;
	////				texCoords[i * 2 + 1] = (texCoords[i * 2 + 1]-minY)/sumY;
	//////				texCoords[i * 2 + 1] = ((texCoords[i * 2 + 1]-minY)/sumY)*3;
	////		}
	//
	//		glGenBuffers(1, &_vbo[2]);
	//		glBindBuffer(GL_ARRAY_BUFFER, _vbo[2]);
	//		glBufferData(GL_ARRAY_BUFFER, 2 * mesh->mNumVertices * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	//		glEnableVertexAttribArray (2);
	//
	//		delete[] texCoords;
	//	}

	//	if(mesh->HasFaces()) {
	//		unsigned int *indices = new unsigned int[mesh->mNumFaces * 3];
	//		for(int i = 0; i < mesh->mNumFaces; ++i) {
	//			indices[i * 3] = mesh->mFaces[i].mIndices[0];
	//			indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
	//			indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
	//		}
	//
	//		glGenBuffers(1, &_vbo[3]);
	//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo[3]);
	//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * mesh->mNumFaces * sizeof(GLuint), indices, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//		glEnableVertexAttribArray (3);
	//
	//		delete[] indices;
	//	}

	//	//TANGENTEN
	//	if(mesh->HasTangentsAndBitangents()) {
	//		float *tangents = new float[mesh->mNumVertices * 3];
	//		for(int i = 0; i < mesh->mNumFaces; ++i) {
	//			tangents[i * 3] = mesh->mTangents[i].x;
	//			tangents[i * 3 + 1] = mesh->mTangents[i].y;
	//			tangents[i * 3 + 2] = mesh->mTangents[i].z;
	////			std::cout<<"has tangents: x: "<<tangents[i * 3]<<" "<<std::endl;
	////			std::cout<<"has tangents: y: "<<tangents[i * 3+1]<<" "<<std::endl;
	////			std::cout<<"has tangents: z: "<<tangents[i * 3+2]<<" "<<std::endl;
	//		}
	//
	//		glGenBuffers(1, &_vbo[4]);
	//		glBindBuffer(GL_ARRAY_BUFFER, _vbo[4]);
	//		glBufferData(GL_ARRAY_BUFFER, 3 * mesh->mNumVertices * sizeof(GLfloat), tangents, GL_STATIC_DRAW);
	//
	//		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	//		glEnableVertexAttribArray (4);
	//
	//		delete[] tangents;
	//	}
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glBindVertexArray(0);
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

	glDrawArrays(GL_TRIANGLES, 0, _elementCount);

	glBindVertexArray(0);
}

//-----------------------------------------------------------------

Model::Model(const char *filename) {
			Assimp::Importer importer;
			const aiScene *scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals | aiProcess_CalcTangentSpace);
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
