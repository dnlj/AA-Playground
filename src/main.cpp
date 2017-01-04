// STD
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <exception>
#include <vector>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

// TinyObjLoader
#define TINYOBJLOADER_IMPLEMENTATION // NOTE: Only define this once
#include <tinyobjloader/tiny_obj_loader.h>

// Playground
#include <Playground/Playground.hpp>
#include <Playground/Vertex.hpp>
#include <Playground/Model.hpp>
#include <Playground/Camera.hpp>

std::ostream& operator<<(std::ostream& stream, const glm::vec3& vec) {
	stream << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return stream;
}

void checkGLErrors(bool displayCheckMessage = false) {
	if (displayCheckMessage) {
		std::cout << "Checking for OpenGL errors...\n";
	}

	for (int err = glGetError(); err != GL_NO_ERROR; err = glGetError()) {
		switch (err) {
			case GL_NO_ERROR:
				// This will never be called, see for loop definition
				std::cout << "GL_NO_ERROR\n";
				break;
			case GL_INVALID_ENUM:
				std::cout << "GL_INVALID_ENUM\n";
				break;
			case GL_INVALID_VALUE:
				std::cout << "GL_INVALID_VALUE\n";
				break;
			case GL_INVALID_OPERATION:
				std::cout << "GL_INVALID_OPERATION\n";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION\n";
				break;
			case GL_OUT_OF_MEMORY:
				std::cout << "GL_OUT_OF_MEMORY\n";
				break;
			case GL_STACK_UNDERFLOW:
				std::cout << "GL_STACK_UNDERFLOW\n";
				break;
			case GL_STACK_OVERFLOW:
				std::cout << "GL_STACK_OVERFLOW\n";
				break;
			default:
				std::cout << "Unknown return value from glGetError()" << std::endl;
				break;
		}
	}
}

void checkShaderSuccess(GLuint shader) {
	GLint success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	
	if (success) { return; }

	GLint maxLength = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

	std::string errorMessage(maxLength, '\0');
	glGetShaderInfoLog(shader, static_cast<GLsizei>(errorMessage.size()), nullptr, &errorMessage[0]);

	std::cout << "[SHADER ERROR] " << errorMessage << "\n";
}

void printInfo() {
	auto vendor = glGetString(GL_VENDOR);
	auto version = glGetString(GL_VERSION);
	auto render = glGetString(GL_RENDERER);
	auto glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

	std::cout << "Vendor: " << vendor << "\n";
	std::cout << "GPU: " << render << "\n";
	std::cout << "OpenGL version: " << version << "\n";
	std::cout << "GLSL version: " << glsl_version << "\n";
}

void run(GLFWwindow* window) {
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	printInfo();

	// General GL stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// Create the color texture for the frame buffer
	GLuint fbColorTexture;
	glGenTextures(1, &fbColorTexture);
	glBindTexture(GL_TEXTURE_2D, fbColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Create the depth texture for the frame buffer
	GLuint fbDepthTexture;
	glGenTextures(1, &fbDepthTexture);
	glBindTexture(GL_TEXTURE_2D, fbDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	// Create frame buffer
	GLuint frameBuffer;
	glCreateFramebuffers(1, &frameBuffer);
	
	// Bind textures to frameBuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbColorTexture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbDepthTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Setup passthrough shaders
	GLuint screenProgram = glCreateProgram();
	{
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		const std::string vertShaderSource = Playground::loadFile("shaders/passthrough_vert.glsl");
		const std::string fragShaderSource = Playground::loadFile("shaders/passthrough_frag.glsl");

		const GLchar* vertShaderSourcePtr = vertShaderSource.c_str();
		const GLchar* fragShaderSourcePtr = fragShaderSource.c_str();

		glShaderSource(vertShader, 1, &vertShaderSourcePtr, nullptr);
		glShaderSource(fragShader, 1, &fragShaderSourcePtr, nullptr);

		glCompileShader(vertShader);
		glCompileShader(fragShader);

		checkShaderSuccess(vertShader);
		checkShaderSuccess(fragShader);

		// Setup passthrough program
		glAttachShader(screenProgram, vertShader);
		glAttachShader(screenProgram, fragShader);

		glLinkProgram(screenProgram);

		// Detach and delete shaders
		glDetachShader(screenProgram, vertShader);
		glDetachShader(screenProgram, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		glBindFragDataLocation(screenProgram, 0, "finalColor");
	}

	// Setup standard shaders
	GLuint modelProgram = glCreateProgram();
	{
		GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

		const std::string vertShaderSource = Playground::loadFile("shaders/vert.glsl");
		const std::string fragShaderSource = Playground::loadFile("shaders/frag.glsl");

		const GLchar* vertShaderSourcePtr = vertShaderSource.c_str();
		const GLchar* fragShaderSourcePtr = fragShaderSource.c_str();

		glShaderSource(vertShader, 1, &vertShaderSourcePtr, nullptr);
		glShaderSource(fragShader, 1, &fragShaderSourcePtr, nullptr);

		glCompileShader(vertShader);
		glCompileShader(fragShader);

		checkShaderSuccess(vertShader);
		checkShaderSuccess(fragShader);

		// Setup program
		glAttachShader(modelProgram, vertShader);
		glAttachShader(modelProgram, fragShader);

		glLinkProgram(modelProgram);
	
		// Detach and delete shaders
		glDetachShader(modelProgram, vertShader);
		glDetachShader(modelProgram, fragShader);

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		glBindFragDataLocation(modelProgram, 0, "finalColor");
	}
	
	//Playground::Model unit_cube{program, "models/unit_cube.obj", 1.0f};
	Playground::Model unit_axes{modelProgram, "models/unit_axes.obj", 1.0f};
	Playground::Model unit_plane{screenProgram, "models/unit_plane.obj", 2.0f}; // Scale by 2 so it goes from -1 to 1 instead of -0.5 to 0.5

	// Perspective
	Playground::Camera camera{window};

	const auto projection = glm::perspective(glm::radians(75.0f), 16.0f/9.0f, 0.01f, 1000.0f);
	const auto model = glm::mat4{};

	// Get uniform locations
	const auto mvpLocation = glGetUniformLocation(modelProgram, "mvp");
	const auto modelMatrixLocation = glGetUniformLocation(modelProgram, "modelMatrix");
	const auto viewPositionLocation = glGetUniformLocation(modelProgram, "viewPosition");
	const auto lightPositionLocation = glGetUniformLocation(modelProgram, "lightPosition");

	const auto colorAttachmentLocation = glGetUniformLocation(screenProgram, "colorAttachment");

	// Constants
	const glm::vec3 lightPosition = {0.0, 0.0, 10.0};

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Update camera and matrices
		camera.update();
		const auto view = camera.getViewMatrix();
		const auto mvp = projection * view * model;

		// Draw into the frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the model program
		glUseProgram(modelProgram);

		// Update uniforms
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);
		glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &model[0][0]);
		glUniform3fv(viewPositionLocation, 1, &camera.getPosition()[0]);
		glUniform3fv(lightPositionLocation, 1, &lightPosition[0]);

		// Draw the model
		glBindVertexArray(unit_axes.getVAO());
		glDrawArrays(GL_TRIANGLES, 0, unit_axes.getCount());

		// Draw onto the screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Use screen program
		glUseProgram(screenProgram);
		
		// Bind fbColorTexture to texture 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbColorTexture);
		glUniform1i(colorAttachmentLocation, 0);
		
		// Draw the frame buffer
		glBindVertexArray(unit_plane.getVAO());
		glDrawArrays(GL_TRIANGLES, 0, unit_plane.getCount());

		// Other
		glfwSwapBuffers(window);
		glfwPollEvents();
		checkGLErrors();

		// Check if we should close the window
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	// Delete OpenGL objects
	glDeleteTextures(1, &fbColorTexture);
	glDeleteTextures(1, &fbDepthTexture);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteProgram(modelProgram);
	glDeleteProgram(screenProgram);
}

	
int main(int argc, char* argv[]) {
	Playground::setup();

	auto window = Playground::getNewWindow("AA Playground");
	run(window);

	return 0;
}