// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Playground
#include <Playground/RendererForward.hpp>
#include <Playground/Playground.hpp>

namespace Playground {
	RendererForward::RendererForward(int width, int height, const std::vector<std::shared_ptr<Playground::Model>>& models) : models{models} {
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

			// Detach and delete shaders
			glDetachShader(modelProgram, vertShader);
			glDetachShader(modelProgram, fragShader);

			glDeleteShader(vertShader);
			glDeleteShader(fragShader);

			glBindFragDataLocation(modelProgram, 0, "finalColor");
		}

		// Setup the models
		for (auto& model : models) {
			model->setupForUseWith(modelProgram);
		}

		// Get uniform locations
		mvpLocation = glGetUniformLocation(modelProgram, "mvp");
		modelMatrixLocation = glGetUniformLocation(modelProgram, "modelMatrix");
		viewPositionLocation = glGetUniformLocation(modelProgram, "viewPosition");
		lightPositionLocation = glGetUniformLocation(modelProgram, "lightPosition");
	};

	RendererForward::~RendererForward() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &fboColorTexture);
		glDeleteTextures(1, &fboDepthTexture);
		glDeleteProgram(modelProgram);
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
		glUniform3fv(lightPositionLocation, 1, &camera.getPosition()[0]);

		for (const auto& model : models) {
			// Update matrices
			glm::mat4 modelMatrix = glm::translate({}, model->getPosition());
			const auto mvp = projection * view * modelMatrix;

			// Update the uniforms
			glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
			glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

			// Draw the model
			glBindVertexArray(model->getVAO());
			glDrawArrays(GL_TRIANGLES, 0, model->getCount());
		}

		// Unbind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

	int RendererForward::getFrameBuffer() const {
		return fbo;
	};
}