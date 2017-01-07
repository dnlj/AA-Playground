// STD
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <exception>
#include <vector>
#include <memory>

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
#include <Playground/Renderer.hpp>
#include <Playground/RendererForward.hpp>
#include <Playground/PointLight.hpp>
#include <Playground/Renderable.hpp>

std::ostream& operator<<(std::ostream& stream, const glm::vec3& vec) {
	stream << "vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return stream;
}

void run(GLFWwindow* window) {
	int windowWidth;
	int windowHeight;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	Playground::printInfo();

	// General GL stuff
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	// Setup our light data
	std::vector<Playground::PointLight> lights {
		{{+2.0f, +0.0f, +2.0f}, {1.0f, 1.0f, 1.0f}, 1.0f},
		{{-2.0f, +0.0f, +2.0f}, {0.0f, 1.0f, 0.0f}, 1.0f},
		{{-2.0f, +0.0f, -2.0f}, {0.0f, 0.0f, 1.0f}, 1.0f},
		{{+2.0f, +0.0f, -2.0f}, {0.0f, 1.0f, 1.0f}, 1.0f},
	};

	// Load our models
	auto modelUnitCube = std::make_shared<Playground::Model>("models/unit_cube.obj", 1.0f);
	auto modelUnitAxes = std::make_shared<Playground::Model>("models/unit_axes.obj", 1.0f);
	auto modelLightBall = std::make_shared<Playground::Model>("models/light_ball.obj", 0.1f);

	// Setup our objects
	std::vector<Playground::Renderable> objects {
		{modelUnitCube, glm::vec3{0.0f, 4.0f, 0.0f}},
		{modelUnitAxes, glm::vec3{0.0f, 0.0f, 0.0f}},
	};

	for (const auto& light : lights) {
		objects.push_back({modelLightBall, light.position});
	}

	// Renderer
	auto renderer = std::make_shared<Playground::RendererForward>(windowWidth, windowHeight, objects, lights);

	// Setup our camera
	Playground::Camera camera{window, 75.0f, 0.01f, 1000.0f};

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Update camera and matrices
		camera.update();

		// Draw the scene
		renderer->draw(camera);

		// Copy over our frame buffer
		glBlitNamedFramebuffer(renderer->getFrameBuffer(), 0, 0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		// Other
		glfwSwapBuffers(window);
		glfwPollEvents();
		Playground::checkGLErrors();

		// Check if we should close the window
		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}
	}
}

	
int main(int argc, char* argv[]) {
	Playground::setup();

	auto window = Playground::getNewWindow("AA Playground");
	run(window);

	return 0;
}