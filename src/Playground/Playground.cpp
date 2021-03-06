// STD
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Playground
#include <Playground/Playground.hpp>

namespace Playground {
	void initializeOpenGL() {
		if (!openGLInitialized) {
			int loaded = ogl_LoadFunctions();
			if (loaded == ogl_LOAD_FAILED) {
				throw std::runtime_error("glLoadGen initialization failed.");
			}

			int numFailed = loaded - ogl_LOAD_SUCCEEDED;
			if (numFailed > 0) {
				std::runtime_error("glLoadGen failed to load " + std::to_string(numFailed) + " functions.");
			}

			if (!ogl_IsVersionGEQ(OPENGL_VERSION_MAJOR, OPENGL_VERSION_MINOR)) {
				throw std::runtime_error("OpenGL version " + std::to_string(OPENGL_VERSION_MAJOR) + "." + std::to_string(OPENGL_VERSION_MINOR) + " is not available.");
			}

			openGLInitialized = true;
		}
	}

	GLFWwindow* getNewWindow(const std::string& title, int width, int height) {
		// Initialize GLFW
		if (!glfwInit()) {
			throw std::runtime_error("GLFW initialization failed.");
		}

		// GLFW hints
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_DECORATED, GL_TRUE);

		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 32);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);

		glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

		// Create the window
		GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

		if (!window) {
			throw std::runtime_error("GLFW failed to create window.");
		}

		// Make the window's context current
		glfwMakeContextCurrent(window);

		// Enable vsync 
		glfwSwapInterval(1);

		// Set resize callback
		glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });

		// Initilize OpenGL
		initializeOpenGL();

		return window;
	}
	
	void setup() {
		if (std::atexit(Playground::cleanup) != 0) {
			throw std::runtime_error("Failed to register Playground::cleanup with std::atexit.");
		}

		// Setup glfw error callback
		glfwSetErrorCallback([](int error, const char* desc) {
			std::stringstream stream;
			stream << "GLFW error 0x"
				<< std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex
				<< error << ": " << desc;

			std::cerr << stream.str() << "\n";
		});
	}

	void cleanup() {
		glfwTerminate();
	}

	std::string loadFile(const std::string &fileName) {
		std::ifstream file(fileName, std::ios::in | std::ios::binary);

		if (!file) {
			throw std::runtime_error("Unable to load file \"" + fileName + "\".");
		}

		std::string ret;

		file.seekg(0, std::ios::end);
		ret.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&ret[0], ret.size());

		return ret;
	}

	void checkGLErrors(bool displayCheckMessage) {
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

		if (success) {
			return;
		}

		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorMessage(maxLength, '\0');
		glGetShaderInfoLog(shader, static_cast<GLsizei>(errorMessage.size()), nullptr, &errorMessage[0]);

		std::cout << "[SHADER ERROR] " << errorMessage << "\n";
	}

	void checkLinkStatus(GLuint program) {
		GLint success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);

		if (success) {
			return;
		}

		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		std::string errorMessage(maxLength, '\0');
		glGetProgramInfoLog(program, static_cast<GLsizei>(errorMessage.size()), nullptr, &errorMessage[0]);

		std::cout << "[LINK ERRROR] " << errorMessage << "\n";
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
}