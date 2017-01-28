// STD
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <exception>
#include <vector>
#include <memory>
#include <random>

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
#include <Playground/AntiAliasingMode.hpp>

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
	std::vector<Playground::PointLight> lights;

	srand(256); // Seed rand so we always get the same results

	for (int x = -8; x < 8; ++x) {
		for (int z = -8; z < 8; ++z) {
			const float scale = 10.0f;
			float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float y = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 20.0f;

			lights.push_back({
				{static_cast<float>(x * scale), y, static_cast<float>(z * scale)},
				{r, g, b},
				5.0f
			});
		}
	}

	// Load our models
	auto modelUnitCube = std::make_shared<Playground::Model>("models/unit_cube.obj", 1.0f);
	auto modelUnitPlane = std::make_shared<Playground::Model>("models/unit_plane_flat.obj", 1000.0f);
	auto modelLightBall = std::make_shared<Playground::Model>("models/light_ball.obj", 0.08f);
	auto modelSponza = std::make_shared<Playground::Model>("models/sponza.obj", 0.05f);

	// Setup our objects
	std::vector<Playground::Renderable> objects {
		{modelUnitCube, glm::vec3{0.0f, 4.0f, 0.0f}},
		{modelUnitPlane, glm::vec3{0.0f, -0.1f, 0.0f}},
		{modelSponza, glm::vec3{0.0f, 0.0f, 0.0f}},
	};

	// Add objects to visualize our lights
	for (const auto& light : lights) {
		objects.push_back({modelLightBall, light.position});
	}

	// Renderer
	auto renderer = std::make_shared<Playground::RendererForward>(windowWidth, windowHeight, Playground::AntiAliasingMode::NONE, 1, 2, objects, lights);


	// Setup our camera
	Playground::Camera camera{window, 75.0f, 0.01f, 1000.0f};

	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Update camera and matrices
		camera.update();

		// Draw the scene
		renderer->draw(camera);

		// Copy over our frame buffer
		glViewport(0, 0, windowWidth, windowHeight);
		glBlitNamedFramebuffer(renderer->getFrameBuffer(), 0,
			0, 0, windowWidth, windowHeight,
			0, 0, windowWidth, windowHeight,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);

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