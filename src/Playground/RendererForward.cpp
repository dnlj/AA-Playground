// STD
#include <iostream>
#include <algorithm>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Playground
#include <Playground/RendererForward.hpp>
#include <Playground/Playground.hpp>

namespace Playground {
	RendererForward::RendererForward(const int width, const int height, const AntiAliasingMode mode, const int power, int screenScale, const std::vector<Renderable>& objects, const std::vector<PointLight>& lights) :
		objects{objects},
		lights{lights},
		lightCount{static_cast<GLuint>(lights.size())},
		fboWidth{width},
		fboHeight{height},
		screenWidth{width},
		screenHeight{height},
		scale{screenScale} {

		if (lightCount > MAX_LIGHTS) {
			std::cout << "[WARNING] The size of \"lights\" must not exceed Playground::MAX_LIGHTS = " << MAX_LIGHTS << ". Clamping.\n";
			lightCount = Playground::MAX_LIGHTS;
		}

		// Load unit plane
		unitPlane = std::make_shared<Model>("models/unit_plane.obj", 2.0f);

		glm::ivec2 maxViewportSize;

		{
			GLint maxTextureSize;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

			std::cout << "maxTextureSize: " << maxTextureSize << "\n";

			GLint maxViewportDims[2];
			glGetIntegerv(GL_MAX_VIEWPORT_DIMS, maxViewportDims);
			std::cout << "maxViewportDims[0]: " << maxViewportDims[0] << "\n";
			std::cout << "maxViewportDims[1]: " << maxViewportDims[1] << "\n";

			maxViewportSize.x = std::min(maxTextureSize, maxViewportDims[0]);
			maxViewportSize.y = std::min(maxTextureSize, maxViewportDims[1]);
		}


		while (screenScale > 1) {
			fboWidth = width * screenScale;
			fboHeight = height * screenScale;

			const bool validWidth = fboWidth <= maxViewportSize.x;
			const bool validHeight = fboHeight <= maxViewportSize.y;

			if (validWidth && validHeight) { break; }

			std::cout << "[WARNING] screen scale " << screenScale << " exceeds maximum viewport size. Decreasing screen scale to ";
			std::cout << --screenScale << ".\n";

			continue;
		}

		if (mode == AntiAliasingMode::NONE) {
		} else {
			std::cout << "[WARNING] Anit aliasing mode \"" << mode << "\" is not supported yet for RendererForward. Not using anti-aliasing\n";
		}
		
		{ // Setup fbo
			// Create the color texture for the frame buffer
			glGenTextures(1, &fboColorTexture);
			glBindTexture(GL_TEXTURE_2D, fboColorTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// Create the depth texture for the frame buffer
			glGenTextures(1, &fboDepthTexture);
			glBindTexture(GL_TEXTURE_2D, fboDepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, fboWidth, fboHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

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
		}

		{ // Setup fboScreen
			// Create the color texture for the frame buffer
			glGenTextures(1, &fboScreenColorTexture);
			glBindTexture(GL_TEXTURE_2D, fboScreenColorTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			// Create frame buffer
			glCreateFramebuffers(1, &fboScreen);

			// Bind textures to frameBuffer
			glBindFramebuffer(GL_FRAMEBUFFER, fboScreen);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboScreenColorTexture, 0);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Setup the model program
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

		// Setup the screen program
		screenProgram = glCreateProgram();
		{
			GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

			const std::string vertShaderSource = loadFile("shaders/forward/super_sample_vert.glsl");
			const std::string fragShaderSource = loadFile("shaders/forward/super_sample_frag.glsl");

			const GLchar* vertShaderSourcePtr = vertShaderSource.c_str();
			const GLchar* fragShaderSourcePtr = fragShaderSource.c_str();

			glShaderSource(vertShader, 1, &vertShaderSourcePtr, nullptr);
			glShaderSource(fragShader, 1, &fragShaderSourcePtr, nullptr);

			glCompileShader(vertShader);
			glCompileShader(fragShader);

			checkShaderSuccess(vertShader);
			checkShaderSuccess(fragShader);

			// Setup program
			glAttachShader(screenProgram, vertShader);
			glAttachShader(screenProgram, fragShader);

			glLinkProgram(screenProgram);

			checkLinkStatus(screenProgram);

			// Detach and delete shaders
			glDetachShader(screenProgram, vertShader);
			glDetachShader(screenProgram, fragShader);

			glDeleteShader(vertShader);
			glDeleteShader(fragShader);
			
			glBindFragDataLocation(screenProgram, 0, "finalColor");
		}

		// Get locations
		mvpLocation = glGetUniformLocation(modelProgram, "mvp");
		modelMatrixLocation = glGetUniformLocation(modelProgram, "modelMatrix");
		viewPositionLocation = glGetUniformLocation(modelProgram, "viewPosition");
		lightCountLocation = glGetUniformLocation(modelProgram, "lightCount");
		colorAttachmentLocation = glGetUniformLocation(screenProgram, "colorAttachment");
		scaleLocation = glGetUniformLocation(screenProgram, "scale");
		
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

		unitPlane->setupForUseWith(screenProgram);
	};

	RendererForward::~RendererForward() {
		glDeleteFramebuffers(1, &fbo);
		glDeleteTextures(1, &fboColorTexture);
		glDeleteTextures(1, &fboDepthTexture);
		glDeleteProgram(modelProgram);
		glDeleteProgram(screenProgram);
		glDeleteBuffers(1, &ubo);
	};

	void RendererForward::draw(const Camera& camera) {
		// Bind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glViewport(0, 0, fboWidth, fboHeight);
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


		// Bind our screen frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, fboScreen);
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT);

		// Use the screen program
		glUseProgram(screenProgram);

		// Activate our textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fboColorTexture);

		// Update uniforms
		glUniform1i(colorAttachmentLocation, 0);
		glUniform1iv(scaleLocation, 1, &scale);

		// Downsample and draw to screen
		glBindVertexArray(unitPlane->getVAO());
		glDrawArrays(GL_TRIANGLES, 0, unitPlane->getCount());

		// Unbind our frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

	int RendererForward::getFrameBuffer() const {
		return fboScreen;
	};
}