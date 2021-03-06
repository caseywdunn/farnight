#include<filesystem>
namespace fs = std::filesystem;
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

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#include<nlohmann/json.hpp>
#include<conio.h>

#define PI 3.1415926538
int glutGet(GLenum state);
unsigned int width = 1280;
unsigned int height = 720;

class O {
	std::vector<std::vector<float>> vertices;
	std::vector<std::vector<int>> indices;
	fs::path texture_path;

	nlohmann::json parse_stl(fs::path stl_path)
	{
		std::ifstream json_file(stl_path);
		nlohmann::json j;
		json_file >> j;

		// Validation
		if (!j["indices"].is_array()) {
			throw std::range_error("invalid indices");
		}
		if (!j["vertices"].is_array()) {
			throw std::range_error("invalid vertices");
		}

		return j;
	}

public:

	O(fs::path stl_path) {
		nlohmann::json j = parse_stl(stl_path);
		vertices = j["vertices"].get<std::vector<std::vector<float>>>();
		indices = j["indices"].get<std::vector<std::vector<int>>>();
		texture_path = j["texture"].get<std::string>();
	}

	std::string summary() {
		std::string s = "\nObject summary\n";

		s += "verteces\n";
		for (int i = 0; i < vertices.size(); i++)
		{
			for (int j = 0; j < vertices[i].size(); j++)
			{
				s += std::to_string(vertices[i][j]) + "\t";
			}
			s += "\n";
		}

		s += "indices\n";
		for (int i = 0; i < indices.size(); i++)
		{
			for (int j = 0; j < indices[i].size(); j++)
			{
				s += std::to_string(indices[i][j]) + "\t";
			}
			s += "\n";
		}

		return s;
	}

	std::vector<GLfloat> get_GL_vertices() {
		std::vector<GLfloat> vertices_flat;
		for (int i = 0; i < vertices.size(); i++)
		{
			for (int j = 0; j < vertices[i].size(); j++)
			{
				vertices_flat.push_back(vertices[i][j]);
			}
		}
		return vertices_flat;
	}

	std::vector<GLuint> get_GL_indices() {
		std::vector<GLuint> indices_flat;
		for (int i = 0; i < indices.size(); i++)
		{
			for (int j = 0; j < indices[i].size(); j++)
			{
				indices_flat.push_back(indices[i][j]);
			}
		}

		return indices_flat;
	}

	fs::path get_texture_path() {
		return texture_path;
	}

};

// From https://www.3dgep.com/understanding-the-view-matrix/#fps-camera
// Pitch must be in the range of [-pi/2 ... pi/2] radians and 
// yaw must be in the range of [0 ... 2pi] radians.
// Pitch and yaw variables must be expressed in radians.
glm::mat4 FPSViewRH(glm::vec3 eye, float pitch, float yaw)
{
	// I assume the values are already converted to radians.
	float cosPitch = cos(pitch);
	float sinPitch = sin(pitch);
	float cosYaw = cos(yaw);
	float sinYaw = sin(yaw);

	glm::vec3 xaxis = { cosYaw, 0, -sinYaw };
	glm::vec3 yaxis = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
	glm::vec3 zaxis = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

	// Create a 4x4 view matrix from the right, up, forward and eye position vectors
	glm::mat4 viewMatrix = {
		glm::vec4(xaxis.x,            yaxis.x,            zaxis.x,      0),
		glm::vec4(xaxis.y,            yaxis.y,            zaxis.y,      0),
		glm::vec4(xaxis.z,            yaxis.z,            zaxis.z,      0),
		glm::vec4(-dot(xaxis, eye), -dot(yaxis, eye), -dot(zaxis, eye), 1)
	};

	return viewMatrix;
}


int main()
{
	// //////////////////////////////////////////////////////
	// Initialize GLFW

	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of width by height pixels, naming it "FarnightOpenGL"
	GLFWwindow* window = glfwCreateWindow(width, height, "FarnightOpenGL", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = width, y = height
	glViewport(0, 0, width, height);

	// Generates Shader object using shaders default.vert and default.frag
	Shader shaderProgram("default.vert", "default.frag");

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");

	// Set input mode
	// https://www.glfw.org/docs/3.1/input.html#input_cursor_mode
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	double prevTime = glfwGetTime();

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);


	// //////////////////////////////////////////////////////
	// Initialize Character controller

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
	glfwGetCursorPos(window, &xpos_old, &ypos_old);
	double mouse_v_x = 0;
	double mouse_v_y = 0;
	double mouse_sensitivity = 0.001f;


	// //////////////////////////////////////////////////////
    // Set up objects

	// Set up paths
	fs::path assets_dir = fs::current_path().fs::path::parent_path() / fs::path("assets");
	fs::path models_dir = assets_dir / fs::path("models");

	// Load the model
	fs::path model_name = "cube.json";
	fs::path model_path = models_dir / model_name;
	std::cout << "Model file:  " << model_path.string() << std::endl;
	O myO(model_path);
	std::cout << myO.summary();

	std::vector<GLfloat> vertices_gl = myO.get_GL_vertices();
	std::vector<GLuint> indices_gl = myO.get_GL_indices();
	fs::path texture_path = myO.get_texture_path();

	// Generates Vertex Array Object and binds it
	VAO VAO1;
	VAO1.Bind();

	// Generates Vertex Buffer Object and links it to vertices
	VBO VBO1(&vertices_gl[0], (sizeof(vertices_gl[0]) * vertices_gl.size()));
	// Generates Element Buffer Object and links it to indices
	EBO EBO1(&indices_gl[0], (sizeof(indices_gl[0]) * indices_gl.size()));

	// Links VBO attributes such as coordinates and colors to VAO
	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	// Unbind all to prevent accidentally modifying them
	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

	// Texture
	fs::path textures_dir = assets_dir / fs::path("textures") / fs::path("blocks");
	fs::path full_texture_path = textures_dir / texture_path;
	std::cout << "Texture file:  " << full_texture_path.string() << std::endl;
	Texture objectTex(full_texture_path.string().c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	objectTex.texUnit(shaderProgram, "tex0", 0);

	// Object location
	float cube_x = 0.0f;
	float cube_y = 0.0f;
	float cube_z = -2.0f;

	// //////////////////////////////////////////////////////
	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// //////////////////////////////////////////////////////
		// Character control
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
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

		// Simple timer for game ticks
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			prevTime = crntTime;

			// Camera orientation
			double xpos, ypos;
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
			//std::cout << "pan: "<< camera_pan_theta/PI << "pi tilt: " << camera_tilt_theta/PI << "pi x: " << camera_x << " y: " << camera_y  << " z: " << camera_z << std::endl;

		}

		// https://learnopengl.com/Getting-started/Camera
		glm::vec3 cameraPos = glm::vec3(
			camera_x,
			camera_y,
			camera_z
		);

		glm::mat4 view = FPSViewRH(cameraPos, camera_tilt_theta, camera_pan_theta);

		if (xpos_old == width) {
			glfwSetCursorPos(window, 0, ypos_old);
		}

		// //////////////////////////////////////////////////////
		// Draw

		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Assigns different transformations to each matrix
		// Transform
		model = glm::translate(model, glm::vec3(cube_x, cube_y, cube_z));
		proj  = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

		// Outputs the matrices into the Vertex Shader
		int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		// Assigns a value to the uniform; NOTE: Must always be done after activating the Shader Program
		glUniform1f(uniID, 0.5f);
		// Binds texture so that is appears in rendering
		objectTex.Bind();
		// Bind the VAO so OpenGL knows to use it
		VAO1.Bind();
		// Draw primitives, number of indices, datatype of indices, index of indices
		glDrawElements(GL_TRIANGLES, (sizeof(indices_gl[0]) * indices_gl.size()) / sizeof(int), GL_UNSIGNED_INT, 0);
		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
	objectTex.Delete();
	shaderProgram.Delete();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}