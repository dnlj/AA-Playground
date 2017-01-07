#pragma once

// GLM
#include <glm/glm.hpp>

namespace Playground {
	class PointLight {
		public:
			glm::vec3 position;
			glm::vec3 color;
			float intensity;
	};
}
