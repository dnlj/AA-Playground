#pragma once

// STD
#include <memory>

// GLM
#include <glm/glm.hpp>

// Playground
#include <Playground/Model.hpp>

namespace Playground {
	class Renderable {
		public:
			std::shared_ptr<Model> model;
			glm::vec3 position;
	};
}
