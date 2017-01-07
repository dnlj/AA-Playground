// STD
#include <iostream>

// TinyObjLoader
#include <tinyobjloader/tiny_obj_loader.h>

// Playground
#include <Playground/Model.hpp>

namespace Playground {
	Model::Model(const std::string& path, const float scale, glm::vec3 color) : vao{0}, vbo{0}, count{0} {

		// Load the obj
		std::vector<Playground::Vertex> data;
		load(path, scale, color, data);

		// Create vao
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Create vbo
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Playground::Vertex), &data[0], GL_STATIC_DRAW);

		// Unbind
		glBindVertexArray(0); // VAO should be unbound before buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	};

	Model::~Model() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	};

	void Model::setupForUseWith(GLuint program) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		// Setup the vertex attributes
		GLint vertPosition = glGetAttribLocation(program, "vertPosition");
		GLint vertNormal = glGetAttribLocation(program, "vertNormal");
		GLint vertColor = glGetAttribLocation(program, "vertColor");
		GLint vertTexCoord = glGetAttribLocation(program, "vertTexCoord");

		if (vertPosition > -1) {
			glEnableVertexAttribArray(vertPosition);
			glVertexAttribPointer(vertPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Playground::Vertex), reinterpret_cast<GLvoid*>(offsetof(Playground::Vertex, position)));
		}

		if (vertNormal > -1) {
			glEnableVertexAttribArray(vertNormal);
			glVertexAttribPointer(vertNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Playground::Vertex), reinterpret_cast<GLvoid*>(offsetof(Playground::Vertex, normal)));
		}

		if (vertColor > -1) {
			glEnableVertexAttribArray(vertColor);
			glVertexAttribPointer(vertColor, 3, GL_FLOAT, GL_FALSE, sizeof(Playground::Vertex), reinterpret_cast<GLvoid*>(offsetof(Playground::Vertex, color)));
		}

		if (vertTexCoord > -1) {
			glEnableVertexAttribArray(vertTexCoord);
			glVertexAttribPointer(vertTexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Playground::Vertex), reinterpret_cast<GLvoid*>(offsetof(Playground::Vertex, texcoord)));
		}

		// Unbind
		glBindVertexArray(0); // VAO should be unbound before buffers
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	GLuint Model::getVAO() {
		return vao;
	};

	GLuint Model::getCount() {
		return count;
	};

	void Model::load(const std::string& path, const float scale, glm::vec3 color, std::vector<Playground::Vertex>& data) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string error;

		// Load the obj
		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, path.c_str())) {
			throw std::runtime_error("TinyObjLoader was unable to load \"" + path + "\" with error: " + error);
		}

		// If tinyobj::LoadObj succeeded this is a warning
		if (!error.empty()) {
			std::cerr << error << std::endl;
		}

		// Load the obj into data
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				Playground::Vertex vertex{};
				
				// Positions
				if (index.vertex_index > -1) {
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0] * scale,
						attrib.vertices[3 * index.vertex_index + 1] * scale,
						attrib.vertices[3 * index.vertex_index + 2] * scale,
					};
				} else {
					std::cout << "[WARNING] Missing normal in model \"" << path << "\"\n";
					vertex.position = {0.0f, 0.0f, 0.0f};
				}
				
				// Normals
				if (index.normal_index > -1) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				} else {
					std::cout << "[WARNING] Missing normal in model \"" << path << "\"\n";
					vertex.normal = {0.0f, 0.0f, 0.0f};
				}
				
				// Color
				vertex.color = color;
				
				// Texture coordinates
				if (index.texcoord_index > -1) {
					vertex.texcoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				} else {
					std::cout << "[WARNING] Missing texture coordinate in model \"" << path << "\"\n";
					vertex.texcoord = {0.0f, 0.0f};
				}
				
				data.push_back(vertex);
				++count;
			}
		}
	}
}