#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../model.h"

class Sphere : public Model {
public:
	Sphere(unsigned int maxNoinstansces)
		: Model("sphere", BoundTypes::SPHERE, maxNoinstansces, NO_TEX | DYNAMIC) { }

	void init() {
		loadModel("assets/models/sphere/scene.gltf");
	}
};

#endif // !SPHERE_HPP
