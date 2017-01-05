// GLM
#include <glm/gtc/matrix_transform.hpp>

// Playground
#include <Playground/Camera.hpp>

namespace Playground {
	Camera::Camera(GLFWwindow* window, float fov, float nearZ, float farZ) :
		window{window},
		orientation{},
		position{0.0f, 0.0f, 0.0f},
		lastMousePos{getMousePos()},
		moveSpeed{0.25f},
		turnScale{0.004f} {
		// Get the window height
		int width;
		int height;
		glfwGetWindowSize(window, &width, &height);

		// Calculate the aspect ratio
		float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fov), aspectRatio, nearZ, farZ);
	}

	glm::vec3 Camera::getRight() const {
		return glm::inverse(orientation) * worldRight;
	}

	glm::vec3 Camera::getUp() const {
		return glm::inverse(orientation) * worldUp;
	}

	glm::vec3 Camera::getForward() const {
		return glm::inverse(orientation) * worldForward;
	}

	void Camera::update() {
		const auto curMousePos = getMousePos();

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2)) {
			const glm::vec2 mouseDelta = {
				curMousePos.x - lastMousePos.x,
				curMousePos.y - lastMousePos.y
			};

			orientation *= glm::angleAxis(mouseDelta.y * turnScale, getRight());
			orientation *= glm::angleAxis(mouseDelta.x * turnScale, worldUp);


			if (glfwGetKey(window, GLFW_KEY_W)) {
				position += getForward() * moveSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_S)) {
				position -= getForward() * moveSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_D)) {
				position += getRight() * moveSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_A)) {
				position -= getRight() * moveSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_SPACE)) {
				position += worldUp * moveSpeed;
			}

			if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL)) {
				position -= worldUp * moveSpeed;
			}

			orientation = glm::normalize(orientation);
		}

		lastMousePos = curMousePos;
	}

	glm::mat4 Camera::getViewMatrix() const {
		return glm::mat4_cast(orientation) * glm::translate(glm::mat4{}, -position);
	}

	glm::mat4 Camera::getProjectionMatrix() const {
		return projection;
	}

	glm::vec3 Camera::getPosition() const {
		return position;
	}

	glm::vec2 Camera::getMousePos() const {
		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		return{static_cast<float>(mouseX), static_cast<float>(mouseY)};
	}
}