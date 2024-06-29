#ifndef HANDLE_INPUT
#define HANDLE_INPUT

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include "scene.h"


class CameraState {
public:
	CameraState();

	glm::vec3 cameraPos() const { return camera_pos; };
	glm::vec3 cameraFront() const { return camera_front; };
	glm::vec3 cameraUp() const { return camera_up; };
	GLfloat deltaTime() { return delta_time; };
	GLfloat lastFrame() { return last_frame; };

	void moveForward();
	void moveBackwards();
	void moveLeft();
	void moveRight();

	void setRegularSpeed() { camera_speed = regular_speed; }
	void setFastSpeed() { camera_speed = fast_speed; }
	void setFront(glm::vec3 front) { camera_front = front; }
	void setTime(GLfloat dt, GLfloat last) { delta_time = dt; last_frame = last; }

private:
	// Camera
	glm::vec3 camera_pos;
	glm::vec3 camera_front;
	glm::vec3 camera_up;
	GLfloat delta_time;  // Time between current frame and last frame
	GLfloat last_frame;  // Time of last frame

	GLfloat camera_speed;
	GLfloat regular_speed;
	GLfloat fast_speed;
};


class MouseState {
public:
	MouseState();

	bool firstMouse() const { return first_mouse; };
	float getYaw() const { return yaw; };
	float getPitch() const { return pitch; };
	float lastX() const { return last_x; };
	float lastY() const { return last_y; };
	float getFov() const { return fov; };
	bool buttonPressed() const { return button_pressed; };
	
	void setLastX(float val) { last_x = val; }
	void setLastY(float val) { last_y = val; }
	void setYaw(float val) { yaw = val; }
	void setPitch(float val) { pitch = val; }
	void setFov(float val) { fov = val; }
	void setFirstMouse(bool val) { first_mouse = val; }
	void setPressed(bool val) { button_pressed = val; }

private:
	// Mouse control
	bool first_mouse;
	float yaw;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
	float pitch;
	float last_x;
	float last_y;
	float fov;


	// Mouse button state
	bool button_pressed;
};


struct WindowData {
	MouseState& mouseState;
	CameraState& cameraState;
};



// Function to handle keyboard input
void processInput(GLFWwindow* window, Scene& scene, CameraState& camera);


// Function to handle mouse movement
void mouse_callback(GLFWwindow* window, MouseState& mouse, CameraState& camera, double xpos, double ypos);


// Function to handle mouse scroll
void scroll_callback(GLFWwindow* window, MouseState& mouse, double xoffset, double yoffset);


// Function to handle mouse button input
void mouse_button_callback(GLFWwindow* window, MouseState& mouse, int button, int action, int mods);


#endif HANDLE_INPUT