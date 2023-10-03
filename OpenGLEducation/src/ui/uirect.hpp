#ifndef UIRECT_HPP
#define UIRECT_HPP

#include <vector>
#include <glm/glm.hpp>

#include "../graphics/model.h"
#include "../graphics/texture.h"
#include "../graphics/material.h"
#include "../graphics/vertexmemory.hpp"

class UIRect {
public:
	UIRect(glm::vec3 color)
		: color(color) {}

	void init() {
		int noVertices = 4;

		vertices = {
		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f
		};

		indices = {
			0, 1, 2, // first triangle
			3, 1, 2  // second triangle
		};

		VAO.generate();
		VAO.bind();
		
	}
private:
	glm::vec3 color;

	ArrayObject VAO;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

#endif // UIRECT_HPP
