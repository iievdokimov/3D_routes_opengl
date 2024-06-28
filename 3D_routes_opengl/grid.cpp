#include "grid.h"

// Function to create the grid lines along X, Y, and Z axes
void createGrid(std::vector<GLfloat>& gridVertices, double size, double step) {
    for (float i = 0; i <= size; i += step) {
        // Parallel to X-axis
        gridVertices.push_back(-0);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);
        gridVertices.push_back(size);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);

        // Parallel to Z-axis
        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(-0);
        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(size);

        // Parallel to Y-axis
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);
        gridVertices.push_back(-0);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(i);
        gridVertices.push_back(size);

        // Parallel to Y-axis
        gridVertices.push_back(0.0f);
        gridVertices.push_back(-0);
        gridVertices.push_back(i);
        gridVertices.push_back(0.0f);
        gridVertices.push_back(size);
        gridVertices.push_back(i);
    }
}