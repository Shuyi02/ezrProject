/*
 * Control.h
 *
 *  Created on: Dec 6, 2015
 *      Author: Shuyi
 */

#ifndef SRC_EXEC_UTILS_CONTROL_H_
#define SRC_EXEC_UTILS_CONTROL_H_

namespace utils {

	void computeMatricesFromInputs();
	glm::mat4 getViewMatrix();
	glm::mat4 getProjectionMatrix();

} /* namespace utils */

#endif /* SRC_EXEC_UTILS_CONTROL_H_ */
