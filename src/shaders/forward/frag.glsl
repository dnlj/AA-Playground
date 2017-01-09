#version 450 core

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
};

const float EPSILON = 0.000001;
const uint MAX_LIGHTS = 1024;

in vec3 fragPosition; // The world space position of this fragment
in vec3 fragNormal; // The normal of this fragment
in vec3 fragColor; // The interpolated fragment color

layout(std140) uniform Lights {
	PointLight lights[MAX_LIGHTS]; // A array of all lights in our scene
};

uniform vec3 viewPosition; // The position of the view/camera
uniform uint lightCount; // The number of lights

out vec4 finalColor; // The final fragment color

vec3 calculatePointLight(PointLight light) {
	// Calculate vectors
	vec3 lightDir = normalize(light.position - fragPosition);
	vec3 viewDir = normalize(viewPosition - fragPosition);

	// Calculate dot products
	float dotNL = max(0.0, dot(fragNormal, lightDir));

	// Calculate lighting factors
	vec3 diffuseLight = fragColor * dotNL;

	// Calculate attenuation
	float attenuation = 1.0 / (EPSILON + pow(distance(light.position, fragPosition), 2.0)); // We add epsilon here to prevent division by zero

	// Calculate final lighting
	return light.intensity * attenuation * light.color * diffuseLight;
}

void main() {
	vec3 totalLighting = vec3(0.0);

	for (uint i = 0; i < lightCount; ++i) {
		totalLighting += calculatePointLight(lights[i]);
	}

	finalColor = vec4(totalLighting, 1.0);
}