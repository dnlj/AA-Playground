#pragma once

// Playground
#include <Playground/Camera.hpp>

namespace Playground {
	class Renderer {
		public:
			Renderer() = default;
			virtual ~Renderer() = default;

			virtual void draw(const Camera& camera) = 0;
			virtual int getFrameBuffer() const = 0;
	};
}