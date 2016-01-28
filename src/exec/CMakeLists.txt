cmake_minimum_required(VERSION 2.8)

project(myEzrHatching)



set(CMAKE_CONFIGURATION_TYPES Debug;Release)

find_package(OpenGL3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)
find_package(Assimp REQUIRED)


IF("${CMAKE_SYSTEM}" MATCHES "Linux")

	SET(ADDITIONAL_LIBRARIES "X11" "Xxf86vm" "pthread") 
	MESSAGE("Additional libs: " "${ADDITIONAL_LIBRARIES}")
ENDIF()

file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

message("Found headers: " "${HEADER_FILES}")
message("Found headers: " "${SOURCE_FILES}")
add_executable(myEzrHatching ${SOURCE_FILES} ${HEADER_FILES})



add_definitions(-DSHADERS_PATH="${SHADERS_PATH}")
add_definitions(-DRESOURCES_PATH="${RESOURCES_PATH}")


include_directories(
    ${OpenGL3_INCLUDE_PATH}
    ${GLEW_INCLUDE_PATH}
    ${GLFW3_INCLUDE_PATH}
    ${GLM_INCLUDE_PATH}
	${ASSIMP_INCLUDE_PATH}
    ${EXTERNAL_LIBRARY_PATHS}
	${DEPENDENCIES_PATH}/
    ${CMAKE_SOURCE_DIR}/src/libraries/
)

target_link_libraries(
    myEzrHatching
    ${GLFW3_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${OpenGL3_LIBRARIES}
    ${ASSIMP_LIBRARIES}
    ${ADDITIONAL_LIBRARIES}
)

message("Assimp include: " "${ASSIMP_INCLUDE_PATH}")
message("Assimp lib: " "${ASSIMP_LIBRARIES}")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
