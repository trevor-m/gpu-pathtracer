#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "PathTracerBuffer.h"

// callback functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// movement
void do_movement();

// window dimensions
const GLuint WIDTH = 512, HEIGHT = 512;

// camera
Camera camera(glm::vec3(50, 52, 295.6));
// input
bool keys[1024];
bool firstMouse = true;
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void RenderQuad();

int main() {
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// create window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "PathTracer", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// turn on GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// tell OpenGL size of rendering window
	glViewport(0, 0, WIDTH, HEIGHT);

	// register callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// disable cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// create shader to render textured quad
	Shader quadShader("quad.vert", "quad.frag");

	// create compute shader for pathtracer
	Shader traceShader("pathtrace.comp");
	PathTracerBuffer traceBuffer(WIDTH, HEIGHT);

	// game loop
	while (!glfwWindowShouldClose(window)) {
		// get deltatime
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		GLfloat fps = 1.0 / deltaTime;
		std::string title = "PathTracer FPS: " + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());
		// check and call events
		glfwPollEvents();
		do_movement();

		// get ray basis from camera
		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), WIDTH / (float)HEIGHT, 0.01f, 1000.0f);
		glm::mat4 invViewProj = glm::inverse(projection * view);
		glm::vec4 ray00 = invViewProj * glm::vec4(-1, -1, 0, 1);
		ray00 /= ray00.w;
		ray00 -= glm::vec4(camera.Position, 0.0f);
		glm::vec4 ray10 = invViewProj * glm::vec4(+1, -1, 0, 1);
		ray10 /= ray10.w;
		ray10 -= glm::vec4(camera.Position, 0.0f);
		glm::vec4 ray01 = invViewProj * glm::vec4(-1, +1, 0, 1);
		ray01 /= ray01.w;
		ray01 -= glm::vec4(camera.Position, 0.0f);
		glm::vec4 ray11 = invViewProj * glm::vec4(+1, +1, 0, 1);
		ray11 /= ray11.w;
		ray11 -= glm::vec4(camera.Position, 0.0f);

		// run path tracer
		traceShader.Use();
		traceBuffer.BindTextureWrite();
		glUniform3fv(glGetUniformLocation(traceShader.Program, "ray00"), 1, glm::value_ptr(glm::vec3(ray00)));
		glUniform3fv(glGetUniformLocation(traceShader.Program, "ray01"), 1, glm::value_ptr(glm::vec3(ray01)));
		glUniform3fv(glGetUniformLocation(traceShader.Program, "ray10"), 1, glm::value_ptr(glm::vec3(ray10)));
		glUniform3fv(glGetUniformLocation(traceShader.Program, "ray11"), 1, glm::value_ptr(glm::vec3(ray11)));
		glUniform3fv(glGetUniformLocation(traceShader.Program, "eye"), 1, glm::value_ptr(camera.Position));
		glUniform1f(glGetUniformLocation(traceShader.Program, "time"), currentFrame);
		glDispatchCompute(WIDTH, HEIGHT, 1);

		// wait for finish
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// render image to screen
		glClear(GL_COLOR_BUFFER_BIT);
		quadShader.Use();
		traceBuffer.BindTextureRead(quadShader, 0);
		RenderQuad();

		// swap buffers
		glfwSwapBuffers(window);
	}

	// exit
	glfwTerminate();
	return 0;
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	// When a user presses the escape key, we set the WindowShouldClose property to true, 
	// closing the application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// RenderQuad() Renders a quad that fills the screen
GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			// Positions        // Texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// Setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}