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
			Model(GLuint program, const std::string& path, const float scale = 1.0f);
			virtual ~Model();

			GLuint getVAO();
			GLuint getCount();

		private:
			GLuint vao;
			GLuint vbo;
			GLuint count;

			void load(const std::string& path, const float scale, std::vector<Vertex>& data);
	};
}