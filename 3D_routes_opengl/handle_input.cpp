#include "handle_input.h"

CameraState::CameraState(){
	camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
	camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
	camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	delta_time = 0.1f;
	last_frame = 0.0f;

	regular_speed = 100.0f;
	fast_speed = 1000.0f;
	camera_speed = regular_speed;
}

void CameraState::moveForward(){
	camera_pos += camera_front * deltaTime() * camera_speed;
}

void CameraState::moveBackwards(){
	camera_pos -= camera_front * deltaTime() * camera_speed;
}

void CameraState::moveLeft(){
	camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed * deltaTime();
}

void CameraState::moveRight() {
	camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed * deltaTime();
}

MouseState::MouseState()
{
	first_mouse = true;
	yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
	pitch = 0.0f;
	last_x = 0;
	last_y = 0;
	fov = 45.0f;
	button_pressed = false;
}

void processInput(GLFWwindow* window, Scene& scene, CameraState& camera)
{

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.moveForward();
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.moveBackwards();
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.moveLeft();
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.moveRight();
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera.setFastSpeed();
	}
	else {
		camera.setRegularSpeed();
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		std::cout << "Frame: " << scene.get_frame_id() << std::endl;
		std::cout << camera.cameraPos().x << " " << camera.cameraPos().y << " " << camera.cameraPos().z << std::endl;
		scene.next_frame();
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		std::cout << "Frame: " << scene.get_frame_id() << std::endl;
		scene.prev_frame();
	}
}

void mouse_callback(GLFWwindow* window, MouseState& mouse, CameraState& camera, double xpos, double ypos)
{
	if (mouse.buttonPressed()) {
		if (mouse.firstMouse()) {
			mouse.setLastX(xpos);
			mouse.setLastY(ypos);
			mouse.setFirstMouse(false);
		}

		float xoffset = xpos - mouse.lastX();
		float yoffset = mouse.lastY() - ypos;  // reversed since y-coordinates go from bottom to top

		mouse.setLastX(xpos);
		mouse.setLastY(ypos);

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		mouse.setYaw(mouse.getYaw() + xoffset);
		mouse.setPitch(mouse.getPitch() + yoffset);

		if (mouse.getPitch() > 89.0f) {
			mouse.setPitch(89.0f);
		}
		if (mouse.getPitch() < -89.0f) {
			mouse.setPitch(-89.0f);
		}

		glm::vec3 front;
		front.x = cos(glm::radians(mouse.getYaw())) * cos(glm::radians(mouse.getPitch()));
		front.y = sin(glm::radians(mouse.getPitch()));
		front.z = sin(glm::radians(mouse.getYaw())) * cos(glm::radians(mouse.getPitch()));

		camera.setFront(glm::normalize(front));
	}
}

void mouse_button_callback(GLFWwindow* window, MouseState& mouse, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouse.setPressed(true);
		}
		else if (action == GLFW_RELEASE) {
			mouse.setPressed(false);
			mouse.setFirstMouse(true);  // Reset the firstMouse flag to avoid a jump when pressing again
		}
	}
}


void scroll_callback(GLFWwindow* window, MouseState& mouse, double xoffset, double yoffset)
{
	if (mouse.getFov() >= 1.0f && mouse.getFov() <= 45.0f) {
		mouse.setFov(mouse.getFov() - yoffset);
	}
	if (mouse.getFov() <= 1.0f) {
		mouse.setFov(1.0f);
	}
	if (mouse.getFov() >= 45.0f) {
		mouse.setFov(45.0f);
	}
}