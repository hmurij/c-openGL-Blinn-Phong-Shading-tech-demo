#include <glad/glad.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <iostream>
#include <iomanip>
#include <bitset>
#include <math.h>
#include <vector>
#include <map>

#include "ShaderProgram.h"
#include "Camera.h"
#include "stb_image.h"
#include "KTX.h"
#include "Object.h"

// Include GLM core features
#include <glm/glm.hpp>																																																																																																													
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// window properties
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// set up transformation matricies
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

// set up camera
glm::vec3 cameraPos = glm::vec3(-4.0f, 0.0f, 25.0f);

Camera camera(cameraPos);

// Camera camera(cameraPos, glm::vec3(0.0, 1.0, 0.0), -90.0, -15.0);

// (glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)

float lastX = (float)SCR_WIDTH / 2, lastY = (float)SCR_HEIGHT / 2;
bool firstMouse = true;
bool freeView = false;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// declare utility functions
void processInput(GLFWwindow* window);
void renderGUI();
std::string matrixToString(std::string matrixName, glm::mat4& matrix);
std::string vec3ToString(std::string vectorName, glm::vec3 vector);
void generate_texture(float* data, int width, int height);
unsigned int loadTexture(const char* path);
static inline float random_float();

void setTextureParamters(GLenum pname, GLint param);
int getGlintParam(const char* param);

// glfw utility functions
void window_size_callback(GLFWwindow* window, int width, int height);
void window_iconify_callback(GLFWwindow* window, int iconified);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
int setUpGLFW(GLFWwindow* window);

bool minized = false;

// const global values
const float PI = 3.1415927f;

// Random number generator
static unsigned int seed = 0x13371337;

// Application global varialbes
bool rotation = false; // leave for ratation pause

bool phong_shading = true;
bool flat = false;

struct
{
	GLuint      color;
	GLuint      normals;
} textures;

struct uniforms_block
{
	glm::mat4     model;
	glm::mat4     view;
	glm::mat4     projection;
};

GLuint          uniforms_buffer;

struct
{
	GLint           diffuse_albedo;
	GLint           specular_albedo;
	GLint           specular_power;
} uniforms[2];

// Light and material properties
		/*uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
		uniform vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
		uniform vec3 specular_albedo = vec3(0.7);
		uniform float specular_power = 128.0;
		uniform vec3 ambient = vec3(0.1, 0.1, 0.1);*/

glm::vec3 diffuse_albedo = glm::vec3(0.5, 0.2, 0.7);
glm::vec3 ambient = glm::vec3(0.1, 0.1, 0.1);
glm::vec3 specular_albedo = glm::vec3(0.7f);
glm::vec3 light_pos = glm::vec3(100.0, 100.0, 100.0);

glm::vec3 rim_color = glm::vec3(0.5, 0.2, 0.7);
float rim_power = 5.0;
float specular_power = 30.0f;

bool ambient_enable = true;
bool diffuse_enable = true;
bool specular_enable = true; 
bool rim_enable = true;

int main(void)
{
	GLFWwindow* window;
	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/
	// glfwWindowHint(GLFW_SAMPLES, 4); // for native MSAA

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL SuperBible Example 13.03 - Blinn-Phong Shading", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	setUpGLFW(window);

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	/*glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);*/

	// set up shader program
	// -------------------------
	// ShaderProgram shader("Resource/Shaders/simple.shader");
	ShaderProgram phong_shading_program("Resource/Shaders/phong_shading.shader");
	ShaderProgram blinn_phong_shading_program("Resource/Shaders/blinn_phong_shading.shader");


	// Set up textures
	// -------------------
	/*unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	ktx::load("Resource/Textures/texture.ktx", texture);*/

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);*/


	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//unsigned int texture;
	//stbi_set_flip_vertically_on_load(true);
	//texture = loadTexture("Resource/Textures/texture.png");


	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &uniforms_buffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniforms_buffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(uniforms_block), NULL, GL_DYNAMIC_DRAW);

	// set up object
	Object object;
	object.load("Resource/Objects/sphere.sbm");


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 450";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Our state
	// bool show_demo_window = true;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// set up variables
		float angle = (float)glfwGetTime();
		angle = angle * 0.25f;

		// leave it for free view camera
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// process input
		processInput(window);
		/* Render here */

		 // leave it for object rotation
		static float total_time = -PI * 6.0f / 7.0f;
		static float last_time = 0;

		if (rotation) {
			total_time += (angle - last_time);
		}
		last_time = angle;

		static const GLfloat ones[] = { 1.0f };
		static const GLfloat gray[] = { 0.2f, 0.2f, 0.2f, 0.0f };

		glClearBufferfv(GL_COLOR, 0, gray);
		glClearBufferfv(GL_DEPTH, 0, ones);

	
		/*vmath::vec3 view_position = vmath::vec3(0.0f, 0.0f, 50.0f);
		vmath::mat4 view_matrix = vmath::lookat(view_position,
			vmath::vec3(0.0f, 0.0f, 0.0f),
			vmath::vec3(0.0f, 1.0f, 0.0f));

		vmath::vec3 light_position = vmath::vec3(20.0f, 20.0f, 0.0f);

		vmath::mat4 light_proj_matrix = vmath::frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 200.0f);
		vmath::mat4 light_view_matrix = vmath::lookat(light_position,
			vmath::vec3(0.0f), vmath::vec3(0.0f, 1.0f, 0.0f));*/

		model = glm::mat4(1.0f);
		model = glm::rotate(model, total_time, glm::vec3(0.0, 1.0, 0.0));
		/*model = glm::rotate(model, total_time, glm::vec3(0.0, 0.7, 0.0));
		model = glm::rotate(model, total_time, glm::vec3(0.4, 0.0, 0.0));
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -4.0f));*/
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

		
		/*per_vertex_program.setMat4("model", model);
		per_vertex_program.setMat4("view", view);
		per_vertex_program.setMat4("projection", projection);*/

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, uniforms_buffer);
		uniforms_block* block = (uniforms_block*)glMapBufferRange(GL_UNIFORM_BUFFER,
			0,
			sizeof(uniforms_block),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		model = glm::scale(model, glm::vec3(7.0));
		
		block->model = model;
		block->view = view;
		block->projection = projection;

		glUnmapBuffer(GL_UNIFORM_BUFFER);

		// Light and material properties
		/*uniform vec3 light_pos = vec3(100.0, 100.0, 100.0);
		uniform vec3 diffuse_albedo = vec3(0.5, 0.2, 0.7);
		uniform vec3 specular_albedo = vec3(0.7);
		uniform float specular_power = 128.0;
		uniform vec3 ambient = vec3(0.1, 0.1, 0.1);*/

		light_pos = glm::vec3(cos(total_time * 2.0f) * 100.0f, 10.0f, sin(total_time * 2.0f) * 100.0f);



		if (phong_shading) {
			phong_shading_program.use();

			phong_shading_program.setFloat("specular_power", specular_power);
			phong_shading_program.setVec3("specular_albedo", specular_albedo);
			phong_shading_program.setVec3("diffuse_albedo", diffuse_albedo);
			phong_shading_program.setVec3("ambient", ambient);

			phong_shading_program.setVec3("viewPos", camera.Position);
			phong_shading_program.setBool("flat_color", flat);

			phong_shading_program.setVec3("rim_color", rim_color);
			phong_shading_program.setFloat("rim_power", rim_power);
			
			phong_shading_program.setVec3("light_pos", light_pos);

			phong_shading_program.setBool("ambient_enable", ambient_enable);
			phong_shading_program.setBool("diffuse_enable", diffuse_enable);
			phong_shading_program.setBool("specular_enable", specular_enable);
			phong_shading_program.setBool("rim_enable", rim_enable);

		}
		else if (!phong_shading) {
			blinn_phong_shading_program.use();

			blinn_phong_shading_program.setFloat("specular_power", specular_power);
			blinn_phong_shading_program.setVec3("specular_albedo", specular_albedo);
			blinn_phong_shading_program.setVec3("diffuse_albedo", diffuse_albedo);
			blinn_phong_shading_program.setVec3("ambient", ambient);

			blinn_phong_shading_program.setVec3("viewPos", camera.Position);
			blinn_phong_shading_program.setBool("flat_color", flat);

			blinn_phong_shading_program.setVec3("rim_color", rim_color);
			blinn_phong_shading_program.setFloat("rim_power", rim_power);

			blinn_phong_shading_program.setVec3("light_pos", light_pos);

			blinn_phong_shading_program.setBool("ambient_enable", ambient_enable);
			blinn_phong_shading_program.setBool("diffuse_enable", diffuse_enable);
			blinn_phong_shading_program.setBool("specular_enable", specular_enable);
			blinn_phong_shading_program.setBool("rim_enable", rim_enable);
		}

		
		// vec3 light_pos = vec3(100.0, 100.0, 100.0);
		

		/*glUniform1f(uniforms[per_vertex ? 1 : 0].specular_power, 30.0f);
		glUniform3fv(uniforms[per_vertex ? 1 : 0].specular_albedo, 1, vmath::vec3(1.0f));*/

		object.render();

		renderGUI();

		// Start the Dear ImGui frame
	   /*ImGui_ImplOpenGL3_NewFrame();
	   ImGui_ImplGlfw_NewFrame();
	   ImGui::NewFrame();*/

	   // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	   /*if (show_demo_window)
		   ImGui::ShowDemoWindow(&show_demo_window);*/

		   // Rendering
		   /*ImGui::Render();
		   ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/

		   /* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	/*glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &buffer);*/
	// glDeleteTextures(2, tex_object);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}


void renderGUI() {

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin(" ");

	ImGui::SetWindowPos(ImVec2(0, 0));

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Separator();
	ImGui::Text("Enable");
	ImGui::Checkbox("Ambient", &ambient_enable);
	ImGui::Checkbox("Diffuse", &diffuse_enable);
	ImGui::Checkbox("Specular", &specular_enable);
	ImGui::Checkbox("Rim", &rim_enable);
	ImGui::Separator();

	ImGui::Checkbox("Rotation", &rotation);
	ImGui::Checkbox("Flat color", &flat);

	ImGui::SliderFloat("Specular power", &specular_power, 0.0f, 256.0f);
	ImGui::ColorEdit3("Specular albedo", &specular_albedo.x);
	ImGui::ColorEdit3("Diffuse albedo", &diffuse_albedo.x);
	ImGui::ColorEdit3("Ambient", &ambient.x);

	ImGui::SliderFloat("Rim power", &rim_power, 0.0f, 128.0f);
	ImGui::ColorEdit3("Rim color", &rim_color.x);

	   
	ImGui::Separator();
	static int shading_type = 1;
	ImGui::RadioButton("Phong shading", &shading_type, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Blinn-Phong Shading", &shading_type, 0);

	if (shading_type) {
		phong_shading = true;
	}
	else if (!shading_type) {
		phong_shading = false;
	}

	ImGui::Separator();

	ImGui::Text("Blend");
	static int blend = 0;
	ImGui::RadioButton("Enable blend", &blend, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Disable blend", &blend, 0);

	if (blend) {
		glEnable(GL_BLEND);
		// glBlendFunc(GL_ONE, GL_ONE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else if (!blend) {
		glDisable(GL_BLEND);
	}

	ImGui::Separator();

	ImGui::Text("Depth test");
	static int depth_test = 1;

	ImGui::RadioButton("Enable Depth test", &depth_test, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Disable Depth test", &depth_test, 0);

	if (depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	ImGui::Separator();

	ImGui::Text("Polygon mode");
	static int polygon_mode = 1;

	ImGui::RadioButton("Line", &polygon_mode, 0);
	ImGui::SameLine();
	ImGui::RadioButton("Fill", &polygon_mode, 1);

	if (polygon_mode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	ImGui::Separator();


	ImGui::Text("Culling");
	static int culling = 1;
	ImGui::RadioButton("Enable culling", &culling, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Disable culling", &culling, 0);

	if (culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	ImGui::Separator();

	ImGui::Text("Winding order");
	static int front_face = 0;
	ImGui::RadioButton("Clock wise", &front_face, 1);
	ImGui::SameLine();
	ImGui::RadioButton("Counter clock wise", &front_face, 0);

	if (front_face)
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);

	ImGui::End();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

// glfw utility functions
int setUpGLFW(GLFWwindow* window) {

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	// set up window callbacks
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetWindowIconifyCallback(window, window_iconify_callback);
	// glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	/*int data;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &data);*/
	// GL_NUM_EXTENSIONS
	/*int number_extensions;

	glGetIntegerv(GL_NUM_EXTENSIONS, &number_extensions);
	char** extension_names = new char*[number_extensions];

	for (int i = 0; i < number_extensions; i++) {
		extension_names[i] = (char*)glGetStringi(GL_EXTENSIONS, i);
	}*/

	std::cout << "-- --------------------------------------------------------------------------------------------- -- \n"
		<< "openGL version: " << glGetString(GL_VERSION) << std::endl
		<< "Renderer : " << glGetString(GL_RENDERER) << std::endl
		<< "Vendor: " << glGetString(GL_VENDOR) << std::endl
		<< "Shading language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl
		/*<< "The number of extensions supported by the GL implementation for the current contex: " << number_extensions << std::endl
		<< "List of supported extensions" << std::endl
		<< "-- --------------------------------------------------------------------------------------------- -- \n";

	for (int i = 0; i < 223; i++) {
		std::cout << std::setw(3) << i << " " << extension_names[i] << std::endl;
	}

	delete [] extension_names;

	std::cout*/
		<< "-- --------------------------------------------------------------------------------------------- -- " << std::endl;

	return 1;
} // end of setUpGLFW


void window_size_callback(GLFWwindow* window, int width, int height)
{
	// std::cout << "window size change" << std::endl;
	if (!minized) {
		SCR_WIDTH = width;
		SCR_HEIGHT = height;
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	}
}

void window_iconify_callback(GLFWwindow* window, int iconified)
{
	if (iconified)
		minized = true;
	else
		minized = false;
}

void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

} // end of processInput

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // reversed since y-coordinates go from bottom to top

	lastX = (float)xpos;
	lastY = (float)ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);


}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (mods == GLFW_MOD_CONTROL && (key == GLFW_KEY_C && action == GLFW_PRESS)) {


		freeView = !freeView;

		if (freeView) {
			// std::cout << "enable free camera" << std::endl;

			glfwSetCursorPosCallback(window, mouse_callback);
			// glfwSetScrollCallback(window, scroll_callback);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		}
		else {
			// std::cout << "disable free camera" << std::endl;
			glfwSetCursorPosCallback(window, NULL);
			glfwSetScrollCallback(window, NULL);

			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			firstMouse = true;

		}

	}

}

std::string matrixToString(std::string matrixName, glm::mat4& matrix)
{
	std::stringstream streamString;
	streamString << matrixName << std::endl;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			streamString << std::showpoint << std::setw(10) << std::fixed << matrix[j][i] << " ";
		}

		streamString << std::endl;
	}

	return streamString.str();
}

std::string vec3ToString(std::string vectorName, glm::vec3 vector)
{
	std::stringstream streamString;
	streamString << vectorName << std::endl
		<< std::showpoint << std::setw(10) << std::fixed << vector.x << " " << vector.y << " " << vector.z << std::endl;

	return streamString.str();
}


void generate_texture(float* data, int width, int height)
{
	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			data[(y * width + x) * 4 + 0] = (float)((x & y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 1] = (float)((x | y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 2] = (float)((x ^ y) & 0xFF) / 255.0f;
			data[(y * width + x) * 4 + 3] = 1.0f;
		}
	}
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}



static inline float random_float()
{
	float res;
	unsigned int tmp;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int*)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}



int getGlintParam(const char* param) {
	int parameter = 0;

	if (strcmp(param, "GL_NEAREST") == 0)
		parameter = GL_NEAREST;
	else if (strcmp(param, "GL_LINEAR") == 0)
		parameter = GL_LINEAR;
	else if (strcmp(param, "GL_NEAREST_MIPMAP_NEAREST") == 0)
		parameter = GL_NEAREST_MIPMAP_NEAREST;
	else if (strcmp(param, "GL_LINEAR_MIPMAP_NEAREST") == 0)
		parameter = GL_LINEAR_MIPMAP_NEAREST;
	else if (strcmp(param, "GL_NEAREST_MIPMAP_LINEAR") == 0)
		parameter = GL_NEAREST_MIPMAP_LINEAR;
	else if (strcmp(param, "GL_LINEAR_MIPMAP_LINEAR") == 0)
		parameter = GL_LINEAR_MIPMAP_LINEAR;

	return parameter;
}

