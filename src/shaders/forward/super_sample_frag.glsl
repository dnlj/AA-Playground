#version 450 core

in vec3 fragPosition; // The screen space position of this fragment
in vec2 fragTexCoord; // The texture coordinates of this vertex

uniform sampler2D colorAttachment; // The texture to sample from
uniform int scale; // The scale of colorAttachment

out vec4 finalColor; // The final fragment color

void main() {
	// Get the size of the colorAttachment texture
	const ivec2 size = textureSize(colorAttachment, 0);

	// Calculate the the uv spacing of one texel
	const vec2 increment = vec2(1.0 / size.x, 1.0 / size.y);

	// Calculate the offset to adjust fragTexCoord by to ensure that it is the bottom left texel of this fragment
	const vec2 offset = vec2(
		mix(0.0, (scale - 1) * increment.x, fragTexCoord.x),
		mix(0.0, (scale - 1) * increment.y, fragTexCoord.y)
	);

	// Calculate the corrected texture coordinates.
	const vec2 fragCoordCorrected = fragTexCoord - offset;

	// Accumulate samples
	vec4 accum = vec4(0.0);

	for (int x = 0; x < scale; ++x) {
		for (int y = 0; y < scale; ++y) {
			vec2 texCoord = fragCoordCorrected + vec2(x * increment.x, y * increment.y);
			accum += texture(colorAttachment, texCoord);
		}
	}

	// Average the accumulated samples
	accum /= scale * scale;
	accum.w = 1.0;

	// Set the final fragment color
	finalColor = accum;
}