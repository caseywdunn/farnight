//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------

#include<stdio.h>
#include<iostream>
#include<string>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"Texture.h"
#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"

#include<nlohmann/json.hpp>
#include<conio.h>

const unsigned int width = 3840;
const unsigned int height = 1920;

char ch;

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

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
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

	// Gets ID of uniform called "scale"
	GLuint uniID = glGetUniformLocation(shaderProgram.ID, "scale");
	
	// Texture
	fs::path textures_dir = assets_dir / fs::path("textures") / fs::path("blocks");
	fs::path full_texture_path = textures_dir / texture_path;
	std::cout << "Texture file:  " << full_texture_path.string() << std::endl;
	Texture objectTex(full_texture_path.string().c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	objectTex.texUnit(shaderProgram, "tex0", 0);

	// Variables that help the rotation of the pyramid
	float rotation_yaw   = 0.0f;
	float rotation_pitch = 0.0f;
	double prevTime = glfwGetTime();

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Variables to control movement
	float delta = 0.75f;
	float delta_yaw = 0.0f;
	float delta_pitch = 0.0f;


	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Process keyboard input
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			delta_yaw = 0.0;
			std::cout << "D" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			delta_yaw = delta;
			std::cout << "D" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			delta_yaw = -delta;
			std::cout << "A" << std::endl;
		}
		else{
			delta_yaw = 0.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			delta_pitch = -delta;
			std::cout << "W" << std::endl;
		}
		else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			delta_pitch = delta;
			std::cout << "S" << std::endl;
		}
		else {
			delta_pitch = 0.0f;
		}

		if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
			rotation_yaw = 0.0f;
			rotation_pitch = 0.0f;
		}


		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Tell OpenGL which Shader Program we want to use
		shaderProgram.Activate();

		// Simple timer
		double crntTime = glfwGetTime();
		if (crntTime - prevTime >= 1 / 60)
		{
			rotation_yaw += delta_yaw;
			rotation_pitch += delta_pitch;
			prevTime = crntTime;
		}

		// Initializes matrices so they are not the null matrix
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 proj = glm::mat4(1.0f);

		// Assigns different transformations to each matrix
		// Transform
		model = glm::rotate(model, glm::radians(rotation_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -2.0f));
		proj = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

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