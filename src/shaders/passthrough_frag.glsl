#version 450 core

in vec3 fragPosition; // The screen space position of this fragment
in vec2 fragTexCoord; // The texture coordinates of this vertex

uniform sampler2D colorAttachment;

out vec4 finalColor; // The final fragment color

void main() {
	finalColor = texture(colorAttachment, fragTexCoord);
}