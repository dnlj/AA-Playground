#version 450 core

in vec3 vertPosition; // The position of this vertex in model space
in vec3 vertNormal; // The normal of this vertex in model space
in vec3 vertColor; // The color of this vertex

uniform mat4 mvp; // The model view projection matrix
uniform mat4 modelMatrix; // The model matrix

out vec3 fragPosition; // The world space position of this fragment
out vec3 fragNormal; // The normal of this fragment
out vec3 fragColor; // The interpolated fragment color


void main() {
	gl_Position = mvp * vec4(vertPosition, 1.0);
	fragPosition = vec3(modelMatrix * vec4(vertPosition, 1.0));

	fragNormal = vertNormal;
	fragColor = vertColor;
}