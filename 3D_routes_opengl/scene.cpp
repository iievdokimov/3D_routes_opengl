#include "scene.h"


FrameData::FrameData(const std::vector<Obstacle>& obst_list, const Obstacle& ship, const Obstacle& reach_target, const std::vector<Vector> trajectory) {
	int sectors = 36, stacks = 18;
	
	// obsts
	for (int i = 0; i < obst_list.size(); ++i) {
		glm::vec3 pos(obst_list[i].pos().x(), obst_list[i].pos().y(), obst_list[i].pos().z());
		generateSphere(obst_list[i].rad(), sectors, stacks, pos, sphereVertices, sphereIndices);
		spherePositions.push_back(pos);
	}

	// controlled object
	controlledObjectPosition = glm::vec3(ship.pos().x(), ship.pos().y(), ship.pos().z());
	generateSphere(ship.rad(), sectors, stacks, controlledObjectPosition, controlledObjectVertices, controlledObjectIndices);

	// trajectory
	std::cout << "traj len: " << trajectory.size() << std::endl;
	for (int i = 1; i < trajectory.size(); ++i) {
		trajectoryVertices.push_back(trajectory[i - 1].x());
		trajectoryVertices.push_back(trajectory[i - 1].y());
		trajectoryVertices.push_back(trajectory[i - 1].z());

		trajectoryVertices.push_back(trajectory[i].x());
		trajectoryVertices.push_back(trajectory[i].y());
		trajectoryVertices.push_back(trajectory[i].z());
	}

	// target
	targetPosition = glm::vec3(reach_target.pos().x(), reach_target.pos().y(), reach_target.pos().z());
	generateSphere(reach_target.rad(), sectors, stacks, targetPosition, targetVertices, targetIndices);

	// vel-ratings

}


Scene::Scene(unsigned int frames) {
	cur_frame_id = 0;
	max_frame_id = frames;

	double data_radius = 400;
	Test test(data_radius);
	Task task = test.get_random_task();

	TrajectoryBuilder builder(task, Hyperparams());
	
	// gen obj
	Obstacle object(task.ship().pos(), task.ship().vel(), task.ship().rad(), task.ship().type(), task.ship().id());


	// target
	double reached_rad = builder.get_hyperparams().target_reached_rad;
	Vector target = task.target();
	Obstacle reach_target(target, {}, reached_rad, ModelType::static_obst, -2);

	std::vector<Vector> object_traj;
	object_traj.push_back(object.pos());
	object_traj.push_back(object.pos());

	// gen frames
	FrameData frame = FrameData(builder.get_obst_list(), object, reach_target, object_traj);
	frame_list.push_back(frame);
	for (int i = 0; i < max_frame_id; ++i) {
		builder.next_step();

		
		object = Obstacle(builder.get_ship().pos(), builder.get_ship().vel(), builder.get_ship().rad(),
			builder.get_ship().type(), builder.get_ship().id());

		object_traj.push_back(object.pos());
		FrameData frame = FrameData(builder.get_obst_list(), object, reach_target, object_traj);
		frame_list.push_back(frame);
	}

}

void Scene::prev_frame()
{
	if (cur_frame_id == 0) {
		return;
	}
	cur_frame_id--;
}



/*
Scene::Scene(unsigned int frames){
	cur_frame_id = 0;
	max_frame_id = frames;

	// example gen obsts
	int num_obsts = 100;
	std::vector<Obstacle> test_obst_list;
	for (int i = 0; i < num_obsts; ++i) {
		int x = i * 5, y = i * 5; int z = i * 5;
		double rad = 1;
		Vector pos(x, y, z);
		Vector vel(0, 0, -1);
		Obstacle obst(pos, vel, rad, ModelType::dynamic_obst, i);

		test_obst_list.push_back(obst);
	}

	// example gen obj
	double rad = 1;
	Vector pos(-2, -2, -2);
	Vector vel(1, 1, 1);
	Obstacle ship(pos, vel, rad, ModelType::dynamic_obst, -1);


	// example gen target
	double reached_rad = 15;
	Vector target(30, 30, 30);
	Obstacle reach_target(target, {}, reached_rad, ModelType::static_obst, -2);

	std::vector<Vector> example_traj;
	for (int i = 0; i < 250; ++i) {
		example_traj.push_back(Vector(i * 5, i * 5, i));
	}

	// gen frames
	FrameData frame = FrameData(test_obst_list, ship, reach_target, example_traj);
	frame_list.push_back(frame);
	for (int i = 0; i < max_frame_id; ++i) {
		std::vector<Obstacle> new_list;
		for (int j = 0; j < test_obst_list.size(); ++j) {
			Vector new_pos = test_obst_list[j].pos().add(test_obst_list[j].vel());
			new_list.push_back(Obstacle(new_pos, test_obst_list[j].vel(), test_obst_list[j].rad(), ModelType::dynamic_obst, test_obst_list[j].id()));
		}
		ship.move(1);
		FrameData frame = FrameData(test_obst_list, ship, reach_target, example_traj);
		frame_list.push_back(frame);
		test_obst_list = new_list;
	}

}
*/