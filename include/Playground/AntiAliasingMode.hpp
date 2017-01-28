#pragma once

// STD
#include <cstdint>
#include <ostream>

namespace Playground {
	enum class AntiAliasingMode : uint8_t {
		NONE, // No anti-aliasing
		MSAA, // Multi sample anti-aliasing
		FXAA, // Fast approximate anti-aliasing
		SMAA, // Subpixel morphological anti-aliasing
		MLAA, // Morphological anti-aliasing
		TAA , // Temporal anti-aliasing
	};
}

std::ostream& operator<<(std::ostream& os, const Playground::AntiAliasingMode mode);