#pragma once

// STD
#include <memory>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Playground
#include <Playground/Renderer.hpp>
#include <Playground/Model.hpp>
#include <Playground/PointLight.hpp>
#include <Playground/Renderable.hpp>
#include <Playground/AntiAliasingMode.hpp>

namespace Playground {
	class RendererForward : public Renderer {
		public:
			RendererForward(const int width, const int height, const AntiAliasingMode mode, const int power, int screenScale, const std::vector<Renderable>& objects, const std::vector<PointLight>& lights);
			virtual ~RendererForward();

			virtual void draw(const Camera& camera) override;
			virtual int getFrameBuffer() const override;

		private:
			GLuint fbo;
			GLuint fboColorTexture;
			GLuint fboDepthTexture;

			GLuint fboScreen;
			GLuint fboScreenColorTexture;

			GLuint modelProgram;
			GLuint screenProgram;
			GLuint ubo;

			GLint mvpLocation;
			GLint modelMatrixLocation;
			GLint viewPositionLocation;
			GLint lightCountLocation;
			GLint colorAttachmentLocation;
			GLint scaleLocation;

			GLuint lightCount;

			int fboWidth;
			int fboHeight;
			int screenWidth;
			int screenHeight;
			int scale;

			const std::vector<Renderable>& objects;
			const std::vector<PointLight>& lights;
			std::shared_ptr<Model> unitPlane;
	};
}