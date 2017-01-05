#pragma once

// STD
#include <string>
#include <vector>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Playground
#include <Playground/Vertex.hpp>

namespace Playground {
	class Model {
		public:
			Model(const std::string& path, const float scale = 1.0f, glm::vec3 position = glm::vec3{0.0f});
			virtual ~Model();

			void setupForUseWith(GLuint program);

			GLuint getVAO();
			GLuint getCount();

			void setPosition(glm::vec3 position);
			const glm::vec3& getPosition() const;

		private:
			GLuint vao;
			GLuint vbo;
			GLuint count;

			glm::vec3 position;

			void load(const std::string& path, const float scale, std::vector<Vertex>& data);
	};
}