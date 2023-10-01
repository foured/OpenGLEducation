#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

#include <vector>
#include <stack>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ft2build.h>

#include "io/keyboard.h"
#include "io/mouse.h"
#include "io/camera.h"

#include "graphics/shader.h"
#include "graphics/texture.h"
#include "graphics/light.h"
#include "graphics/model.h"
#include "graphics/cubemap.h"
#include "graphics/framememory.hpp"

#include "graphics/models/cube.hpp"
#include "graphics/models/lamp.hpp"
#include "graphics/models/gun.hpp"
#include "graphics/models/sphere.hpp"
#include "graphics/models/box.hpp"
#include "graphics//models/plane.hpp"

#include "physics/environment.h"

#include "algorithms/states.hpp"

#include "scene.h"

Scene scene;

void processInput(double dt);
void renderScene(Shader shader);

Camera cam;

glm::mat4 transform = glm::mat4(1.0f);

unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float sensitivity = 0.1f;

Sphere sphere(10);
Cube cube(10);

glm::vec3 baseColor = glm::vec3(1.0f, 1.0f, 1.0f);
/*
	int main   |======================================================================================|
*/
int main() {
	std::cout << "Program started." << std::endl;
	std::cout << "" << std::endl;

	scene = Scene(3, 3, "OpenGL education.", 800, 600);
	if (!scene.init()) {
		std::cout << "Could not open the window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	scene.cameras.push_back(&cam);
	scene.activeCamera = 0;

	/*
		shaders   |==================================================================================|
	*/

	Shader lampShader("assets/shaders/instanced/instanced.vs", "assets/shaders/lamp.fs");
	Shader shader("assets/shaders/instanced/instanced.vs", "assets/shaders/object.fs");
	Shader boxShader("assets/shaders/instanced/box.vs", "assets/shaders/instanced/box.fs");
	Shader textShader("assets/shaders/text.vs", "assets/shaders/text.fs");
	Shader dirLightShader("assets/shaders/shadows/directionalshadow.vs", "assets/shaders/shadows/shadow.fs");

	Shader outlineShader("assets/shaders/outline.vs", "assets/shaders/outline.fs");
	Shader bufferShader("assets/shaders/buffer.vs", "assets/shaders/buffer.fs");
	//Shader skyboxShader("assets/skybox/skybox.vs", "assets/skybox/sky.fs");

	//skyboxShader.activate();
	//skyboxShader.set3Float("min", 0.047f, 0.016f, 0.239f);
	//skyboxShader.set3Float("max", 0.945f, 1.000f, 0.682f);

	/*
		skybox |=====================================================================================|
	*/

	//Cubemap skybox;
	//skybox.init();
	////skybox.loadTextures("assets/skybox");

	/*
		models   |===================================================================================|
	*/

	//Lamp lamp(4);
	//scene.registerModel(&lamp);

	scene.registerModel(&sphere);

	scene.registerModel(&cube);

	Box box;
	box.init();

	DirLight dirLight(
		glm::vec3(-0.2f, -0.9f, -0.2f),
		glm::vec4(glm::vec3(0.1f), 1.0f),
		glm::vec4(glm::vec3(0.6f), 1.0f),
		glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
		BoundingRegion(glm::vec3(-20.0f, -20.0f, 0.5f), glm::vec3(20.0f, 20.0f, 20.0f)));

	//setup plane
	Plane map;
	map.init(dirLight.shadowFBO.textures[0]);
	scene.registerModel(&map);

	//load all model data
	scene.loadModels();


	/*
		lights	|===================================================================================|
	*/

	scene.dirLight = &dirLight;

	//glm::vec3 pointLightPositions[] = {
	//	glm::vec3(0.7f,  0.2f,  2.0f),
	//	glm::vec3(2.3f, -3.3f, -4.0f),
	//	glm::vec3(-4.0f,  2.0f, -12.0f),
	//	glm::vec3(0.0f,  0.0f, -3.0f)
	//};

	//glm::vec4 ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	//glm::vec4 diffuse = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
	//glm::vec4 specular = glm::vec4(1.0f);
	//float k0 = 1.0f;
	//float k1 = 0.09f;
	//float k2 = 0.032f;

	//PointLight pointLights[4];

	//for (unsigned int i = 0; i < 4; i++) {
	//	pointLights[i] = {
	//		pointLightPositions[i],
	//		k0, k1, k2,
	//		ambient, diffuse, specular
	//	};

	//	scene.generateInstance(lamp.id, glm::vec3(0.25f), 0.25f, pointLightPositions[i]),
	//	scene.pointLights.push_back(&pointLights[i]);
	//	States::activate(&scene.activePointLights, i);
	//}

	//SpotLight spotLight = {
	//	cam.cameraPos, cam.cameraFront,
	//	glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(20.0f)),
	//	1.0f, 0.07f, 0.032f,  
	//	glm::vec4(glm::vec3(0.0f), 1.0f), glm::vec4(1.0f), glm::vec4(1.0f)
	//};

	//scene.spotLights.push_back(&spotLight);
	//scene.activeSpotLights = 1;

	glm::vec3 cubePositions[] = {
	   { 1.0f, 3.0f, -5.0f },
	   { -7.25f, 2.1f, 1.5f },
	   { -15.0f, 2.55f, 9.0f },
	   { 4.0f, -3.5f, 5.0f },
	   { 2.8f, 1.9f, -6.2f },
	   { 3.5f, 6.3f, -1.0f },
	   { -3.4f, 10.9f, -5.5f },
	   { 10.0f, -2.0f, 13.2f },
	   { 2.1f, 7.9f, -8.3f },
	};
	for (unsigned int i = 0; i < 9; i++) {
		scene.generateInstance(cube.id, glm::vec3(0.5f), 1.0f, cubePositions[i]);
	}

	States::toggleIndex(&scene.activeSpotLights, 0); //off the spotlight

	scene.generateInstance(cube.id, glm::vec3(20.0f, 0.1f, 20.0f), 100.0f, glm::vec3(0.0f, -3.0f, 0.0f));
	
	//instantiate texture quad
	scene.generateInstance(map.id, glm::vec3(2.0f, 2.0f, 0.0f), 0.0f, glm::vec3(0.0f));

	//insatntiate instances
	scene.initInstances();

	scene.prepare(box);

	scene.variableLog["time"] = (double)0.0;

	scene.defaultFBO.bind(); // rebind default framebuffer
	/*
		mian loop	|===================================================================================|
	*/
	while (!scene.shouldClose()) {
		double currentTime = glfwGetTime();
		deltaTime = currentTime - lastFrame;
		lastFrame = currentTime;

		scene.variableLog["time"] += deltaTime;
		scene.variableLog["fps"] = 1 / deltaTime;
		
		//update csreen valeus
		scene.update();

		processInput(deltaTime);

		//text
		/*scene.variableLog["X"] = scene.cameraPos.x;
		scene.variableLog["Y"] = scene.cameraPos.y;
		scene.variableLog["Z"] = scene.cameraPos.z;

		scene.renderText("courer", textShader, "X: " + scene.variableLog["X"].dump() + " || Y: " + scene.variableLog["Y"].dump() + " || Z: " + scene.variableLog["Z"].dump(), 10.0f, 10.0f, glm::vec2(1.0f), baseColor);
		scene.renderText("courer", textShader, "Time: " + scene.variableLog["time"].dump(), 10.0f, 25.0f, glm::vec2(1.0f), baseColor);
		scene.renderText("courer", textShader, "FPS: " + scene.variableLog["fps"].dump(), 10.0f, 40.0f, glm::vec2(1.0f), baseColor);*/

		//remove launched objs if to far
		for (int i = 0; i < sphere.currentNoInstances; i++) {
			if (glm::length(cam.cameraPos - sphere.instances[i]->pos) >= 100.0f){
				scene.markForDeletion(sphere.instances[i]->instanceId);
			}
		}

		if (scene.variableLog["dispOutline"].val<bool>()) {
			glStencilMask(0x00);
		}

		//render scene to dirlight FBO
		dirLight.shadowFBO.activate();
		scene.renderDirLightShader(dirLightShader);
		renderScene(dirLightShader);

		//render scene normally
		scene.defaultFBO.activate();
		scene.renderShader(shader);
		renderScene(shader);

		//render boxes
		//scene.renderShader(boxShader, false);
		//box.render(boxShader);

		//send new frame
		scene.newFrame(box);
		scene.clearDeadInstnces();
	}

	//skybox.cleanup();
	scene.cleanup();
	return 0;
}

void renderScene(Shader shader) {
	if (sphere.currentNoInstances > 0) {
		scene.renderInstances(sphere.id, shader, deltaTime);
	}
	scene.renderInstances(cube.id, shader, deltaTime);
}

void launchItem() {
	RigidBody* rb = scene.generateInstance(sphere.id, glm::vec3(0.5f), 1.0f, cam.cameraPos);
	if (rb) {
		// instance generated
		rb->transferEnergy(100.0f, cam.cameraFront);
		rb->applyAcceleration(Environment::gravitationalAcceleration);
	}
}

void processInput(double dt) {
	scene.processInput(dt);

	//if (States::isIndexActive(&scene.activeSpotLights, 0)) {
	//	scene.spotLights[0]->position = scene.getActiveCamera()->cameraPos;
	//	scene.spotLights[0]->direction = scene.getActiveCamera()->cameraFront;
	//}

	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		scene.setShouldClose(true);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_L)) {
		//spotLightOn = !spotLightOn;
		States::toggleIndex(&scene.activeSpotLights, 0);
	}

	if (Keyboard::keyWentDown(GLFW_KEY_F)) {
		launchItem();
	}
}
