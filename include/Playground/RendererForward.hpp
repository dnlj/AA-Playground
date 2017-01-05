#pragma once

// STD
#include <memory>

// glLoadGen
#include <glloadgen/gl_core_4_5.h>

// Playground
#include <Playground/Renderer.hpp>
#include <Playground/Model.hpp>

namespace Playground {
	class RendererForward : public Renderer {
		public:
			RendererForward(int width, int height, const std::vector<std::shared_ptr<Model>>& models);
			virtual ~RendererForward();

			virtual void draw(const Camera& camera) override;
			virtual int getFrameBuffer() const override;

		private:
			GLuint fbo;
			GLuint fboColorTexture;
			GLuint fboDepthTexture;
			GLuint modelProgram;
			GLint mvpLocation;
			GLint modelMatrixLocation;
			GLint viewPositionLocation;
			GLint lightPositionLocation;

			const std::vector<std::shared_ptr<Model>>& models;
	};
}