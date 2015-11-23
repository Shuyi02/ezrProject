cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake CACHE PATH "Project specific path. Set manually if it was not found.")
set(DEPENDENCIES_PATH ${CMAKE_SOURCE_DIR}/dep CACHE PATH "Project specific path. Set manually if it was not found.")
set(SHADERS_PATH ${CMAKE_SOURCE_DIR}/src/shaders CACHE PATH "Project specific path. Set manually if it was not found.")
set(RESOURCES_PATH ${CMAKE_SOURCE_DIR}/resources CACHE PATH "Project specific path. Set manually if it was not found.")

add_subdirectory(${CMAKE_SOURCE_DIR}/src/exec)

add_subdirectory(${CMAKE_SOURCE_DIR}/src/shaders)