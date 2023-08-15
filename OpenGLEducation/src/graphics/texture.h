#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

#include <assimp/scene.h>

class Texture {
public:

	//init with name
	Texture(std::string name);

	Texture();
	Texture(std::string dir, std::string path, aiTextureType type);

	void generate();
	void load(bool flip = true);
	void allocate(GLenum format, GLuint width, GLuint height, GLuint type);
	static void setParams(GLenum texMinFilter = GL_NEAREST, GLenum TexMagFilter = GL_NEAREST, GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT);
	void bind();
	void cleanup();

	unsigned int id;
	aiTextureType type;
	std::string name;
	std::string dir;
	std::string path;
};

#endif
