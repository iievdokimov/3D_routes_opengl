#ifndef DRAW_PRIMITIVES_H
#define DRAW_PRIMITIVES_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


// Function to generate sphere data
void generateSphere(float radius, int sectors, int stacks, glm::vec3 Pos, std::vector<GLfloat>& sphereVertices, std::vector<GLuint>& sphereIndices);



#endif //DRAW_PRIMITIVES_H