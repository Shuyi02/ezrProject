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

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

#include "Utils.h"

namespace utils {

/**
*	Constructor, loading the specified aiMesh
**/
Model::MeshEntry::MeshEntry(aiMesh *mesh) {

	//	Vertices = vbo[0]
	//	Normals = vbo[1]
	//	TexCoord = vbo[2]

	_vbo[0] = NULL;
	_vbo[1] = NULL;
	_vbo[2] = NULL;

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	_elementCount = mesh->mNumFaces * 3;

	//------------------------------no use of indexbuffer
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

			glm::vec3 n0 = glm::vec3(mesh->mNormals[first].x, mesh->mNormals[first].y, mesh->mNormals[first].z);
			glm::vec3 n1 = glm::vec3(mesh->mNormals[second].x, mesh->mNormals[second].y, mesh->mNormals[second].z);
			glm::vec3 n2 = glm::vec3(mesh->mNormals[third].x, mesh->mNormals[third].y, mesh->mNormals[third].z);
			normals.push_back(n0);
			normals.push_back(n1);
			normals.push_back(n2);

			//curvature
			glm::vec3 curvatureDirection;
			getCurvatureTensor(a, b, c, n0, n1, n2, curvatureDirection);

			//rotate texture into same direction as a direction (e.g. curvature)
			glm::vec3 globalDir(0.0, 1.0, 0.0);
			glm::vec2 u1;
			glm::vec2 u2;
			glm::vec2 u3;
//			calcTexCoord(globalDir, a, b, c, u1, u2, u3);
			calcTexCoord(curvatureDirection, a, b, c, u1, u2, u3);

			texCoords.push_back(u1);
			texCoords.push_back(u2);
			texCoords.push_back(u3);
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

void Model::MeshEntry::calcTexCoord(glm::vec3 textureDir, glm::vec3 triangleA,
		glm::vec3 triangleB, glm::vec3 triangleC,
		glm::vec2& u1, glm::vec2& u2,glm::vec2& u3) {

	//project the direction of curvature/global direction into plane
	glm::vec3 proj = projectIntoPlane(textureDir, triangleA, triangleB, triangleC);

	//project triangle and everything into2D
	glm::mat3x2 projMat = createProjection(triangleA, triangleB, triangleC);
	glm::vec2 a2D = projMat * (triangleA - triangleA);
	glm::vec2 b2D = projMat * (triangleB - triangleA);
	glm::vec2 c2D = projMat * (triangleC - triangleA);
	glm::vec2 proj2D = glm::normalize(projMat * proj);

	//2DAxis
	glm::vec2 xAxis2D(1.0,0.0);
	glm::vec2 yAxis2D(0.0,1.0);

	//calc texcoords
	u1 = glm::vec2(0.2,0.2);
	u2 = glm::vec2(0.8,0.2);
	float angle2D = acos(glm::dot(glm::normalize(c2D), xAxis2D));
	float lengthV = (glm::length(c2D-a2D)*glm::length(u2-u1))/glm::length(b2D-a2D);
	u3 = glm::vec2(cos(angle2D),sin(angle2D))*lengthV + u1;

	//calc rotation of texture/triangle
	glm::vec2 triangleSide = glm::normalize(glm::vec2(b2D-a2D));
	float cosAlph = glm::dot(triangleSide,proj2D);
	float angle = -acos(cosAlph);

	glm::vec2 rotationPoint = u1;

	//TODO maybe check if normal and curvature/tangent are the same
	//if the same, than do something else or there will be no texture

	glm::mat2 rotate = glm::mat2(cos(angle), sin(angle),
			-sin(angle), cos(angle));

	u1 = rotationPoint + (rotate * (u1-rotationPoint));
	u2 = rotationPoint + (rotate * (u2-rotationPoint));
	u3 = rotationPoint + (rotate * (u3-rotationPoint));
}

void Model::MeshEntry::getCurvatureTensor(glm::vec3 triangleA,
				glm::vec3 triangleB, glm::vec3 triangleC, glm::vec3 normalA,
				glm::vec3 normalB, glm::vec3 normalC, glm::vec3& curvatureDirection){

	//Variablen Name mit _uv ist im Paper (uv) und xn ist (x tiefgestelltes n)

	//equation 9
	float u = 0.5;
	float v = 0.5;
	glm::vec3 x = triangleA + u*(triangleB-triangleA) + v*(triangleC-triangleA);
	glm::vec3 n = normalA + u*(normalB-normalA) + v*(normalC-normalA);

	//equation 10
	glm::vec3 xu_uv = triangleB - triangleA;
	glm::vec3 xv_uv = triangleC - triangleA;

	glm::vec3 n_uv = glm::normalize(n);
	float normN = glm::length(n);
	glm::vec3 partU = normalB-normalA;
	glm::vec3 partV = normalC-normalA;
	glm::vec3 nu = partU/normN - (n*(glm::dot(n,partU))/(normN*normN*normN));
	glm::vec3 nv = partV/normN - (n*(glm::dot(n,partV))/(normN*normN*normN));

	//equation 11
	glm::vec3 xu = xu_uv - (glm::dot(n_uv,xu_uv) * n_uv);
	glm::vec3 xv = xv_uv - (glm::dot(n_uv,xu_uv) * n_uv);

	//equation 1
	float e = glm::dot(xu, xu);
	float f = glm::dot(xu, xv);
	float g = glm::dot(xv, xv);

	//equation 2

	float l = -glm::dot(nu, xu);
	float m1 = -glm::dot(nu, xv);

	//equation 3
	float m2 = -glm::dot(nv, xu);
	// o = paper's N
	float o = -glm::dot(nv, xv);

	//equation 4
	float denominator = e*g - f*f;
	if(glm::abs(denominator) < 1E-7)
	{
		//std::cout << "miau miau miau"<< std::endl;
		curvatureDirection[0] =0.0;
		curvatureDirection[1] = 0.0;
		curvatureDirection[2] = 0.0;
		return;
	}

	//weingarten matrix
	float wein11 = ((l*g) - (m1*f)) / denominator;
	float wein12 = ((m2*g) - (o*f)) / denominator;
	float wein21 = ((m1*e) - (l*f)) / denominator;
	float wein22 = ((o*e) - (m2*f)) / denominator;

	//calc eigenvector
	Eigen::Matrix2f w;
	w << wein11, wein12, wein21, wein22;
	Eigen::EigenSolver<Eigen::Matrix2f> es(w);
	Eigen::Vector2f w1 = es.eigenvectors().col(0).real();
	Eigen::Vector2f w2 = es.eigenvectors().col(1).real();

	//TODO choose which direction (currently always choose the biggest)
	Eigen::Vector2f eigVec;
	if(es.eigenvalues()[0].real() > es.eigenvalues()[1].real()){
		eigVec = w1;
	}else{
		eigVec = w2;
	}
	//calc principal direction
	glm::vec3 k1 = eigVec[0] * xu + eigVec[1] * xv;
//	glm::vec3 k2 = w1[0] * xu + w1[1] * xv;

	//result
	curvatureDirection = k1;
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
