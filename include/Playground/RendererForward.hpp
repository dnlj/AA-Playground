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

namespace Playground {
	class RendererForward : public Renderer {
		public:
			RendererForward(int width, int height, const std::vector<Renderable>& objects, const std::vector<PointLight>& lights);
			virtual ~RendererForward();

			virtual void draw(const Camera& camera) override;
			virtual int getFrameBuffer() const override;

		private:
			GLuint fbo;
			GLuint fboColorTexture;
			GLuint fboDepthTexture;
			GLuint modelProgram;
			GLuint ubo;
			GLint mvpLocation;
			GLint modelMatrixLocation;
			GLint viewPositionLocation;
			GLint lightCountLocation;

			GLuint lightCount;

			const std::vector<Renderable>& objects;
			const std::vector<PointLight>& lights;
	};
}