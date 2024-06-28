#ifndef SCENE_H
#define SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "drawing_primitives.h"

class Obstacle {
public:
	Obstacle(double _x, double _y, double _z, double _rad) {
		x = _x;
		y = _y;
		z = _z;
		rad = _rad;
	}
	double x, y, z, rad;
};

class Vector {
public:
	double x, y, z;
};


class FrameData {
public:
	//const std::vector<GLfloat>& sphereVertices() const;
	FrameData(const std::vector<Obstacle>& obst_list, const Obstacle& ship, const std::vector<Vector> trajectory){
		int sectors = 36, stacks = 18;
		for (int i = 0; i < obst_list.size(); ++i) {
			glm::vec3 pos(obst_list[i].x, obst_list[i].y, obst_list[i].z);
			generateSphere(obst_list[i].rad, sectors, stacks, pos, sphereVertices, sphereIndices);
			spherePositions.push_back(pos);
		}
	
	}

//private:
	// Sphere data
	std::vector<GLfloat> sphereVertices;
	std::vector<GLuint> sphereIndices;
	std::vector<glm::vec3> spherePositions;

	// Grid data
	std::vector<GLfloat> gridVertices;
};


class Scene {
public:
	Scene(unsigned int frames=100) {
		cur_frame_id = 0;
		max_frame_id = frames;

		// example gen
		float rad = 1;
		
		int num_obsts = 100;
		std::vector<Obstacle> test_obst_list;
		for (int i = 0; i < num_obsts; ++i) {
			int x = i * 5, y = i * 5; int z = i * 5;
			double rad = 1;
			Obstacle obst(x, y, z, rad);

			test_obst_list.push_back(obst);
		}
		Obstacle ship(0, 0, 0, 5.5);
		FrameData frame = FrameData(test_obst_list, ship, {});
		frame_list.push_back(frame);
		for (int i = 0; i < max_frame_id; ++i) {
			std::vector<Obstacle> new_list;
			for (int j = 0; j < test_obst_list.size(); ++j) {
				int x = test_obst_list[j].x + 1;
				int y = test_obst_list[j].y + 1;
				int z = test_obst_list[j].z;
				double r = test_obst_list[j].rad;
				new_list.push_back(Obstacle(x, y, z, r));
			}
			FrameData frame = FrameData(test_obst_list, ship, {});
			frame_list.push_back(frame);
			test_obst_list = new_list;
		}

	}
	
	const FrameData& cur_frame() const { return frame_list[cur_frame_id]; }
	void next_frame() { cur_frame_id = std::min(max_frame_id - 1, cur_frame_id + 1); }


private:
	unsigned int cur_frame_id;
	unsigned int max_frame_id;
	std::vector<FrameData> frame_list;
	//obst_list
};



#endif