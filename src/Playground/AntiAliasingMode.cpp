// STD
#include <string>

// Playground
#include <Playground/AntiAliasingMode.hpp>

std::ostream& operator<<(std::ostream& os, const Playground::AntiAliasingMode mode) {
	std::string str;

	switch (mode) {
		case Playground::AntiAliasingMode::NONE:
			str = "Playground::AntiAliasingMode::NONE";
			break;
		case Playground::AntiAliasingMode::MSAA:
			str = "Playground::AntiAliasingMode::MSAA";
			break;
		case Playground::AntiAliasingMode::FXAA:
			str = "Playground::AntiAliasingMode::FXAA";
			break;
		case Playground::AntiAliasingMode::SMAA:
			str = "Playground::AntiAliasingMode::SMAA";
			break;
		case Playground::AntiAliasingMode::MLAA:
			str = "Playground::AntiAliasingMode::MLAA";
			break;
		case Playground::AntiAliasingMode::TAA:
			str = "Playground::AntiAliasingMode::TAA";
			break;
		default:
			str = "[TODO] Add ostream support for Playground::AntiAliasingMode::???? = "
				+ std::to_string(static_cast<std::underlying_type_t<Playground::AntiAliasingMode>>(mode));
			break;
	}

	os << str;
	return os;
}