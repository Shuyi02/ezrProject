/*
 * Utils.h
 *
 *  Created on: Jan 24, 2016
 *      Author: Shuyi
 */

#ifndef SRC_EXEC_UTILS_UTILS_H_
#define SRC_EXEC_UTILS_UTILS_H_


#include <glm/glm.hpp>

namespace utils {

glm::vec3 projectIntoPlane(glm::vec3 globalDirection, glm::vec3 a, glm::vec3 b, glm::vec3 c);
glm::mat3x2 createProjection(glm::vec3 a, glm::vec3 b, glm::vec3 c);

} /* namespace utils */

#endif /* SRC_EXEC_UTILS_UTILS_H_ */
