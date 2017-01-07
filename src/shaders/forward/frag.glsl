#version 450 core

struct PointLight {
	vec3 position;
	vec3 color;
	float intensity;
};

const float SPECULAR_POWER = 20.0;
const float EPSILON = 0.000001;
const uint LIGHT_COUNT = 4;

in vec3 fragPosition; // The world space position of this fragment
in vec3 fragNormal; // The normal of this fragment
in vec3 fragColor; // The interpolated fragment color

uniform PointLight lights[LIGHT_COUNT];
uniform vec3 viewPosition; // The position of the view/camera

out vec4 finalColor; // The final fragment color

vec3 calculatePointLight(PointLight light) {
	// Calculate vectors
	vec3 lightDir = normalize(light.position - fragPosition);
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 halfDir = normalize(lightDir + viewDir);

	// Calculate dot products
	float dotNL = max(0.0, dot(fragNormal, lightDir));
	float dotNH = max(0.0, dot(fragNormal, halfDir));

	// Calculate lighting factors
	vec3 ambientLight = fragColor * 0.05; // 0.05 was choses arbitrarily
	vec3 diffuseLight = light.color * fragColor * dotNL;
	vec3 specularLight = light.color * pow(dotNH, SPECULAR_POWER);

	// Calculate attenuation
	float attenuation = 1.0 / (EPSILON + pow(distance(light.position, fragPosition), 2.0)); // We add epsilon here to prevent division by zero

	// Calculate final lighting
	return light.intensity * attenuation * (ambientLight + diffuseLight + specularLight);
}

void main() {
	vec3 totalLighting = vec3(0.0);

	for (uint i = 0; i < LIGHT_COUNT; ++i) {
		totalLighting += calculatePointLight(lights[i]);
	}

	finalColor = vec4(totalLighting, 1.0);
}