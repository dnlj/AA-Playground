#version 450 core

in vec3 fragPosition; // The world space position of this fragment
in vec3 fragNormal; // The normal of this fragment
in vec3 fragColor; // The interpolated fragment color

uniform vec3 lightPosition; // The position of the light
uniform vec3 viewPosition; // The position of the view/camera

out vec4 finalColor; // The final fragment color

const vec3 specularColor = vec3(1.0, 1.0, 1.0);
const float specularPower = 20.0;

void main() {
	vec3 lightDir = normalize(lightPosition - fragPosition);
	vec3 viewDir = normalize(viewPosition - fragPosition);
	vec3 halfDir = normalize(lightDir + viewDir);

	float dotNL = max(0.0, dot(fragNormal, lightDir));
	float dotNH = max(0.0, dot(fragNormal, halfDir));
	
	vec3 ambientLight = fragColor * 0.05;
	vec3 diffuseLight = fragColor * dotNL;
	vec3 specularLight = specularColor * pow(dotNH, specularPower);

	vec3 totalLighting = ambientLight + diffuseLight + specularLight;
	finalColor = vec4(totalLighting, 1.0);
}