// STD
#include <iostream>

// TinyObjLoader
#include <tinyobjloader/tiny_obj_loader.h>

// Playground
#include <Playground/Model.hpp>

namespace Playground {
	Model::Model(GLuint program, const std::string& path, const float scale) : vao{0}, vbo{0}, count{0} {

		// Load the obj
		std::vector<Playground::Vertex> data;
		load(path, scale, data);

		// Create vao
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Create vbo
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(Playground::Vertex), &data[0], GL_STATIC_DRAW);

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

		// Unbind vao
		glBindVertexArray(0);
	};

	Model::~Model() {
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	};

	GLuint Model::getVAO() {
		return vao;
	};

	GLuint Model::getCount() {
		return count;
	};

	void Model::load(const std::string& path, const float scale, std::vector<Playground::Vertex>& data) {
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
		//for (const auto& shape : shapes) {
		//	for (const auto& index : shape.mesh.indices) {
		//		Playground::Vertex vertex{};
		//
		//		vertex.position = {
		//			attrib.vertices[3 * index.vertex_index + 0] * scale,
		//			attrib.vertices[3 * index.vertex_index + 1] * scale,
		//			attrib.vertices[3 * index.vertex_index + 2] * scale,
		//		};
		//
		//		vertex.normal = {
		//			attrib.normals[3 * index.normal_index + 0],
		//			attrib.normals[3 * index.normal_index + 1],
		//			attrib.normals[3 * index.normal_index + 2],
		//		};
		//
		//		vertex.color = {1.0f, 0.0f, 0.0f};
		//
		//		vertex.texcoord = {
		//			attrib.texcoords[2 * index.texcoord_index + 0],
		//			attrib.texcoords[2 * index.texcoord_index + 1],
		//		};
		//
		//		data.push_back(vertex);
		//		++count;
		//	}
		//}

		// Loop over shapes
		for (size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				int fv = shapes[s].mesh.num_face_vertices[f];

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];
					Playground::Vertex vertex{};
					
					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0] * scale,
						attrib.vertices[3 * index.vertex_index + 1] * scale,
						attrib.vertices[3 * index.vertex_index + 2] * scale,
					};
					
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
					
					vertex.color = {1.0f, 0.0f, 0.0f};
					
					if (index.texcoord_index >= 0) {
						vertex.texcoord = {
							attrib.texcoords[2 * index.texcoord_index + 0],
							attrib.texcoords[2 * index.texcoord_index + 1],
						};
					} else {
						// If no texcoords are given just fill it with zero
						vertex.texcoord = {0.0f, 0.0f};
					}
					
					data.push_back(vertex);
					++count;
				}
				index_offset += fv;

				// per-face material
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}