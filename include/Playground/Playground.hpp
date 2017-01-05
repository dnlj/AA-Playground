#pragma once

// STD
#include <string>

// GLFW
#include <GLFW/glfw3.h>

namespace Playground {
	constexpr int OPENGL_VERSION_MAJOR = 4;
	constexpr int OPENGL_VERSION_MINOR = 5;

	namespace {
		bool openGLInitialized = false;
	}

	void initializeOpenGL();
	GLFWwindow* getNewWindow(const std::string& title = "", int width = 1280, int height = 720);
	void setup();
	void cleanup();
	std::string loadFile(const std::string &fileName);
	void checkGLErrors(bool displayCheckMessage = false);
	void checkShaderSuccess(GLuint shader);
	void printInfo();
}