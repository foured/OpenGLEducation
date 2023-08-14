#ifndef PLANE_HPP
#define PLANE_HPP

#include "../model.h"

class Plane : public Model {
public:
	Plane()
		: Model("plane", BoundTypes::AABB, 1, CONST_INSTANCES) {}

	void init(Texture tex) {
		int noVertices = 4;

		float quadVertices[] = {
			// position			normal				texcoord
			 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
			-0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
		};
	}
};

#endif