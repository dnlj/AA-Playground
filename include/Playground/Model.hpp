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
			Model(const std::string& path, const float scale = 1.0f, glm::vec3 color = {1.0f, 1.0f, 1.0f});
			virtual ~Model();

			void setupForUseWith(GLuint program);

			GLuint getVAO();
			GLuint getCount();

		private:
			GLuint vao;
			GLuint vbo;
			GLuint count;

			void load(const std::string& path, const float scale, glm::vec3 color, std::vector<Vertex>& data);
	};
}