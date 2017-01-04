#pragma once

// GLM
#include <glm/glm.hpp>

namespace Playground {
	class Vertex {
		public:
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec3 color;
			glm::vec2 texcoord;
	};
}