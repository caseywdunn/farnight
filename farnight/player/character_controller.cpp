#include<stdio.h>
#include<iostream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/string_cast.hpp>

#define PI 3.1415926538

class Movement {

	// Variables to control movement
	float delta = 0.75f;

	// Camera angle
	float camera_pan_theta = 0.0f;
	float camera_tilt_theta = 0.0f;

	// Camera position
	float camera_x = 0.0f;
	float camera_y = 0.0f;
	float camera_z = 0.0f;
	float camera_step = 0.1f;
	int camera_forward = 0;
	int camera_right = 0;
	int camera_up = 0;

	// Mouse polling
	double xpos_old, ypos_old;
	int glfwGetCursorPos(window, &xpos_old, &ypos_old);
	double mouse_v_x = 0;
	double mouse_v_y = 0;
	double mouse_sensitivity = 0.001f;

	double xpos, ypos;

	void movement()
	{

		// //////////////////////////////////////////////////////
	// Initialize Character controller


		// //////////////////////////////////////////////////////
			// Character control

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera_right = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			camera_right = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			camera_right = -1;
		}
		else {
			camera_right = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera_forward = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			camera_forward = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera_forward = -1;
		}
		else {
			camera_forward = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera_up = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			camera_up = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
			camera_up = -1;
		}
		else {
			camera_up = 0;
		}

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			camera_pan_theta = 0.0f;
			camera_tilt_theta = 0.0f;
			camera_x = 0.0f;
			camera_y = 0.0f;
			camera_z = 0.0f;
		}

		// Camera orientation
		glfwGetCursorPos(window, &xpos, &ypos);

		mouse_v_x = xpos - xpos_old;
		xpos_old = xpos;

		mouse_v_y = ypos - ypos_old;
		ypos_old = ypos;

		camera_pan_theta -= mouse_v_x * mouse_sensitivity;

		double new_camera_tilt_theta = camera_tilt_theta - mouse_v_y * mouse_sensitivity;
		if (new_camera_tilt_theta >= -PI / 2 && new_camera_tilt_theta <= PI / 2) {
			camera_tilt_theta = new_camera_tilt_theta;
		}

		// Camera position
		float camera_x_delta =
			(sin(camera_pan_theta) * camera_step * (-camera_forward)) +
			(cos(camera_pan_theta) * camera_step * (camera_right));
		float camera_y_delta = 0;
		float camera_z_delta =
			(cos(camera_pan_theta) * camera_step * (-camera_forward)) +
			(sin(camera_pan_theta) * camera_step * (-camera_right));

		camera_x += camera_x_delta;
		camera_y += camera_y_delta;
		camera_z += camera_z_delta;
		std::cout << "pan: " << camera_pan_theta / PI << "pi tilt: " << camera_tilt_theta / PI << "pi x: " << camera_x << " y: " << camera_y << " z: " << camera_z << std::endl;

		// https://learnopengl.com/Getting-started/Camera
		glm::vec3 cameraPos = glm::vec3(
			camera_x,
			camera_y,
			camera_z
		);

		if (xpos_old == width) {
			glfwSetCursorPos(window, 0, ypos_old);
		}
	}

	public:
		void movement();
		glm::vec3 cameraPos;
		float camera_pan_theta = 0.0f;
		float camera_tilt_theta = 0.0f;

};
