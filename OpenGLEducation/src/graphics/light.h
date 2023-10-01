#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"
#include "../algorithms/bounds.h"
#include "framememory.hpp"

struct PointLight {
	glm::vec3 position;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	void render(Shader shader, int idx); // ��� render � ���������
};

struct DirLight {
	glm::vec3 direction;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	//bounding region for shadows
	BoundingRegion br;
	glm::mat4 lightSpaceMatrix;
	FramebufferObject shadowFBO;

	DirLight(glm::vec3 direction, 
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		BoundingRegion br);

	void render(Shader shader, unsigned int textureIdx);

	void updateMatrices();
};

struct SpotLight {
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 up;

	float cutOff;
	float outerCutOff;

	// attenuation constants
	float k0;
	float k1;
	float k2;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	//bounds for the shadows
	float nearPlane;
	float farPlane;

	//light space transformation
	glm::mat4 lightSpaceMatrix;
	
	FramebufferObject shadowFBO;
	
	//constructor
	SpotLight(glm::vec3 positiont, glm::vec3 direction, glm::vec3 up,
		float cutOff, float outerCutOff,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void render(Shader shader, int idx, unsigned int textureIdx);

	void updateMatrices();
};

#endif
