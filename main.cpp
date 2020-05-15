#include <iostream>
#include <iomanip>
#include <cmath>
#include <complex>

#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "ResourceManager.h"
#include "Shader.h"

#define DllExport __declspec(dllexport)

#ifdef _WIN32
#include <Windows.h>

extern "C" {
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

#endif

using namespace std::complex_literals;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

const int iterations = 50;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const float ASPECT = (double)SCREEN_WIDTH / SCREEN_HEIGHT;
glm::vec2 current_screen_size(SCREEN_WIDTH, SCREEN_HEIGHT);

bool Mandelbrot(const std::complex<double> c) {
	std::complex<double> z(0,0);

	double last_magnitude = 0;
	int converge_count = 0;

	for (int i = 0; i < iterations; i++) {
		z = (z * z) + c;
		auto mag = std::abs(z);
		if (mag == INFINITY) {
			printf("Break on infinity\n");
			return false;
		}
		if (mag <= last_magnitude)
			converge_count++;
		last_magnitude = mag;
		printf("%f+%fi | %f\n", z.real(), z.imag(), mag);
	}
	printf("Iterations: %d\nConverges: %d\n", iterations, converge_count);
	return (double(converge_count) / iterations) > 0.2;
}

glm::vec2 position = glm::vec2(-1, 0);
float scale = 1;

int main(int argc, char* argv[]) {	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();

	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(message_callback, 0);

	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	Shader shader = ResourceManager::LoadShader("Shaders/novertex.vs", "Shaders/mandelbrot.fs", nullptr, "mandelbrot");

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	while (!glfwWindowShouldClose(window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		shader.SetInteger("iterations", 800);
		shader.SetVector2f("screenSize", current_screen_size);
		shader.SetFloat("time", currentFrame);
		shader.SetVector2f("user_pos", position);
		shader.SetFloat("user_scale", scale);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		std::this_thread::sleep_for(std::chrono::milliseconds(50));

		glfwSwapBuffers(window);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	const float step = 0.1;
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_UP:
			position.y += step / scale*2;
			break;
		case GLFW_KEY_DOWN:
			position.y -= step / scale*2;
			break;
		case GLFW_KEY_LEFT:
			position.x -= step / scale*2;
			break;
		case GLFW_KEY_RIGHT:
			position.x += step / scale*2;
			break;
		case GLFW_KEY_Q:
			scale -= step * 2;
			break;
		case GLFW_KEY_E:
			scale += step * 2;
			break;
		}
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	int w = height * ASPECT;
	int left = (width - w) / 2;
	glViewport(left, 0, w, height);
	current_screen_size.x = w;
	current_screen_size.y = height;
}

void message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s/n", (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity, message);
}