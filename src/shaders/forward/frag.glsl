#version 450 core

in vec3 fragPosition; // The world space position of this fragment
in vec3 fragNormal; // The normal of this fragment
in vec3 fragColor; // The interpolated fragment color

uniform vec3 lightPosition; // The position of the light
uniform vec3 viewPosition; // The position of the view/camera

out vec4 finalColor; // The final fragment color

const vec3 specularColor = vec3(1.0, 1.0, 1.0);
const float specularPower = 20.0;
const float epsilon = 0.000001;

void main() {
	// Calculate vectors
	vec3 lightDir = normalize(lightPosition - fragPosition);
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 halfDir = normalize(lightDir + viewDir);

	// Calculate dot products
	float dotNL = max(0.0, dot(fragNormal, lightDir));
	float dotNH = max(0.0, dot(fragNormal, halfDir));
	
	// Calculate lighting factors
	vec3 ambientLight = fragColor * 0.05; // 0.05 was choses arbitrarily
	vec3 diffuseLight = fragColor * dotNL;
	vec3 specularLight = specularColor * pow(dotNH, specularPower);

	// Calculate attenuation
	float attenuation = 1.0 / (epsilon + pow(distance(lightPosition, fragPosition), 2.0)); // We add epsilon here to prevent division by zero

	// Calculate final lighting
	vec3 totalLighting = attenuation * (ambientLight + diffuseLight + specularLight);

	//
	finalColor = vec4(totalLighting, 1.0);
}