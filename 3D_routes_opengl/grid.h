#ifndef GRID_H
#define GRID_H


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// Function to create the grid lines along X, Y, and Z axes
void createGrid(std::vector<GLfloat>& gridVertices, double size = 1000.0f, double step = 10.0f);


#endif