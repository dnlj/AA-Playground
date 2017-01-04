#pragma once

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Playground {
	class Camera {
		public:
			Camera(GLFWwindow* window);

			glm::vec3 getRight() const;
			glm::vec3 getUp() const;
			glm::vec3 getForward() const;

			void update();

			glm::mat4 getViewMatrix() const;
			glm::vec3 getPosition() const;

		private:
			static constexpr glm::vec3 worldRight = {1.0f, 0.0f, 0.0f};
			static constexpr glm::vec3 worldUp = {0.0f, 1.0f, 0.0f};
			static constexpr glm::vec3 worldForward = {0.0f, 0.0f, -1.0f};

			GLFWwindow* window;
			glm::quat orientation;
			glm::vec3 position;
			glm::vec2 lastMousePos;
			float moveSpeed;
			float turnScale;

			glm::vec2 getMousePos() const;
	};
}