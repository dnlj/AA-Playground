// STD
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Playground
#include <Playground/RendererForward.hpp>
#include <Playground/Playground.hpp>

namespace Playground {
	RendererForward::RendererForward(int width, int height, const std::vector<Renderable>& objects, const std::vector<PointLight>& lights) :
		objects{objects},
		lights{lights},
		lightCount{static_cast<GLuint>(lights.size())} {

		if (lightCount > MAX_LIGHTS) {
			std::cout << "[WARNING] The size of \"lights\" must not exceed Playground::MAX_LIGHTS = " << MAX_LIGHTS << ". Clamping.\n";
			lightCount = Playground::MAX_LIGHTS;
		}

		// Create the color texture for the frame buffer
		glGenTextures(1, &fboColorTexture);
		glBindTexture(GL_TEXTURE_2D, fboColorTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create the depth texture for the frame buffer
		glGenTextures(1, &fboDepthTexture);
		glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// Create frame buffer
		glCreateFramebuffers(1, &fbo);

		// Bind textures to frameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTexture, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTexture, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Setup shader
		modelProgram = glCreateProgram();
		{
			GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

			const std::string vertShaderSource = loadFile("shaders/forward/vert.glsl");
			const std::string fragShaderSource = loadFile("shaders/forward/frag.glsl");

			const GLchar* vertShaderSourcePtr = vertShaderSource.c_str();
			const GLchar* fragShaderSourcePtr = fragShaderSource.c_str();

			glShaderSource(vertShader, 1, &vertShaderSourcePtr, nullptr);
			glShaderSource(fragShader, 1, &fragShaderSourcePtr, nullptr);

			glCompileShader(vertShader);
			glCompileShader(fragShader);

			checkShaderSuccess(vertShader);
			checkShaderSuccess(fragShader);

			// Setup program
			glAttachShader(modelProgram, vertShader);
			glAttachShader(modelProgram, fragShader);

			glLinkProgram(modelProgram);
			
			checkLinkStatus(modelProgram);

			// Detach and delete shaders
			glDetachShader(modelProgram, vertShader);
			glDetachShader(modelProgram, fragShader);

			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			glBindFragDataLocation(modelProgram, 0, "finalColor");
		}

		// Get locations
		mvpLocation = glGetUniformLocation(modelProgram, "mvp");
		modelMatrixLocation = glGetUniformLocation(modelProgram, "modelMatrix");
		viewPositionLocation = glGetUniformLocation(modelProgram, "viewPosition");
		lightCountLocation = glGetUniformLocation(modelProgram, "lightCount");
		
		// Setup lights UBO
		GLsizeiptr pointLightSize = sizeof(PointLight) + sizeof(GLfloat); // We need to add the extra sizeof(Glfloat) here for padding
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, pointLightSize * lightCount, nullptr, GL_STATIC_DRAW);

		// Set UBO data
		{
			GLfloat tempData[8];
			for (unsigned int i = 0; i < lightCount; ++i) {
				// Position
				tempData[0] = lights[i].position.x;
				tempData[1] = lights[i].position.y;
				tempData[2] = lights[i].position.z;
				tempData[3] = 0; // Padding

				// Color
				tempData[4] = lights[i].color.r;
				tempData[5] = lights[i].color.g;
				tempData[6] = lights[i].color.b;

				// Intensity
				tempData[7] = lights[i].intensity;

				// Set the data
				glBufferSubData(GL_UNIFORM_BUFFER, i * pointLightSize, pointLightSize, tempData);
			}
		}
		
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Bind our UBO to binding point 0
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

		// Bind "Lights" (which is at index lightsIndex) from modelProgram to binding point 0
		GLuint lightsIndex = glGetUniformBlockIndex(modelProgram, "Lights");
		glUniformBlockBinding(modelProgram, lightsIndex, 0);
		
		// Setup the models
		for (auto& obj : objects) {
			obj.model->setupForUseWith(modelProgram);
		}
	};

	RendererForward::~RendererForward() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &fboColorTexture);
		glDeleteTextures(1, &fboDepthTexture);
		glDeleteProgram(modelProgram);
		glDeleteBuffers(1, &ubo);
	};

	void RendererForward::draw(const Camera& camera) {
		// Bind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Get camera matrices
		const auto projection = camera.getProjectionMatrix();
		const auto view = camera.getViewMatrix();

		// Use the model program
		glUseProgram(modelProgram);

		// Update uniforms
		glUniform3fv(viewPositionLocation, 1, &camera.getPosition()[0]);
		glUniform1uiv(lightCountLocation, 1, &lightCount);

		// Draw the models
		for (const auto& obj : objects) {
			// Update matrices
			glm::mat4 modelMatrix = glm::translate({}, obj.position);
			const auto mvp = projection * view * modelMatrix;

			// Update the uniforms
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

			// Draw the model
			glBindVertexArray(obj.model->getVAO());
			glDrawArrays(GL_TRIANGLES, 0, obj.model->getCount());
		}

		// Unbind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

	int RendererForward::getFrameBuffer() const {
		return fbo;
	};
}