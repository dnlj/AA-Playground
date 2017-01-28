#version 450 core

in vec3 vertPosition; // The position of this vertex in screen space
in vec2 vertTexCoord; // The texture coordinates of this vertex

out vec3 fragPosition; // The world space position of this fragment
out vec2 fragTexCoord; // The texture coordinates of this vertex

void main() {
	gl_Position = vec4(vertPosition, 1.0);
	fragTexCoord = vertTexCoord;
}