/*
 * Utils.cpp
 *
 *  Created on: Jan 24, 2016
 *      Author: Shuyi
 */


#include "Utils.h"

namespace utils {

glm::vec3 projectIntoPlane(glm::vec3 globalDirection, glm::vec3 a, glm::vec3 b, glm::vec3 c){
	//calc normal
	glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(b-a), glm::vec3(c-a)));
	//project
	return glm::normalize(globalDirection - (glm::dot(normal,globalDirection)*normal));
}

/*
 * return: actually in math a 2x3 matrix, but glm first says columns then rows
 */
glm::mat3x2 createProjection(glm::vec3 a, glm::vec3 b, glm::vec3 c){
	//2d coord for triangle
	glm::vec3 x3D = b-a;
	glm::vec3 y3D = c-a;
	glm::vec3 xAxis3D = glm::normalize(x3D);
	glm::vec3 yAxis3D = glm::normalize(y3D-(glm::dot(y3D,xAxis3D)*xAxis3D));
	return glm::mat3x2(xAxis3D.x, yAxis3D.x,
			xAxis3D.y, yAxis3D.y,
			xAxis3D.z, yAxis3D.z);
}

} /* namespace utils */
