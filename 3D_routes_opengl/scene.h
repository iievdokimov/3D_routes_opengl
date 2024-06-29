#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "drawing_primitives.h"

#include "models.h"
#include "test_tools.h"
#include "task.h"
#include "config.h"


class FrameData {
public:
	//const std::vector<GLfloat>& sphereVertices() const;
	FrameData(const std::vector<Obstacle>& obst_list, const Obstacle& ship, const Obstacle& reach_target, const std::vector<Vector> trajectory);

//private:
	// Obsts data
	std::vector<GLfloat> sphereVertices;
	std::vector<GLuint> sphereIndices;
	std::vector<glm::vec3> spherePositions;
	std::vector<GLfloat> sphereRads;

	// Controlled Object data
	std::vector<GLfloat> controlledObjectVertices;
	std::vector<GLuint> controlledObjectIndices;
	glm::vec3 controlledObjectPosition;
	GLfloat controlledObjectRad;

	// Target data
	std::vector<GLfloat> targetVertices;
	std::vector<GLuint> targetIndices;
	glm::vec3 targetPosition;
	GLfloat targetRad;


	// Trajectory data
	std::vector<GLfloat> trajectoryVertices;

	// Grid data
	std::vector<GLfloat> gridVertices;
};


class Scene {
public:
	Scene(unsigned int frames = 1000);
	
	const FrameData& cur_frame() const { return frame_list[cur_frame_id]; }
	unsigned int get_frame_id() const { return cur_frame_id; }
	void next_frame() { cur_frame_id = std::min(max_frame_id - 1, cur_frame_id + 1); }
	void prev_frame();


private:
	unsigned int cur_frame_id;
	unsigned int max_frame_id;
	std::vector<FrameData> frame_list;
	//obst_list
};



#endif