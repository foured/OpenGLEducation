#include "mesh.h"

std::vector<Vertex> Vertex::genList(float* vertices, int noVertices) {
	std::vector<Vertex> ret(noVertices);
	int stride = sizeof(Vertex) / sizeof(float);

	for (int i = 0; i < noVertices; i++){
		ret[i].pos = glm::vec3(
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2]
		);

		ret[i].normal = glm::vec3(
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5]
		);

		ret[i].texCoord = glm::vec2(
			vertices[i * stride + 6],
			vertices[i * stride + 7]
		);
	}

	return ret;
}

// default constructor
Mesh::Mesh() {}

// initialize as textured object
Mesh::Mesh(BoundingRegion br, std::vector<Texture> textures)
	: br(br), textures(textures), noTex(false) {}

// initialize as material object
Mesh::Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec)
	: br(br), diffuse(diff), specular(spec), noTex(true) {}

// load vertex and index data
void Mesh::loadData(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, bool pad) {
	this->vertices = _vertices;
	this->indices = _indices;

	// bind VAO
	VAO.generate();
	VAO.bind();

	// generate/set EBO
	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

	// load data into vertex buffers
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].generate();
	VAO["VBO"].bind();

	unsigned int size = this->vertices.size();
	if (pad && size) {
		size++;
	}

	VAO["VBO"].setData<Vertex>(size, &this->vertices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	VAO["VBO"].bind();
	// vertex Positions
	VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
	// normal ray
	VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
	// vertex texture coords
	VAO["VBO"].setAttPointer<GLfloat>(2, 3, GL_FLOAT, 8, 6);

	VAO["VBO"].clear();

	ArrayObject::clear();
}
void Mesh::render(Shader shader, unsigned int noInstances) {
	if (noTex) {
		shader.set4Float("material.diffuse", diffuse);
		shader.set4Float("material.specular", specular);
		shader.setInt("noTex", 1);
	}
	else {


		unsigned int diffuseIdx = 0;
		unsigned int specularIdx = 0;

		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			std::string name;
			switch (textures[i].type)
			{
			case aiTextureType_DIFFUSE:
				name = "diffuse" + std::to_string(diffuseIdx++);
				break;

			case aiTextureType_SPECULAR:
				name = "specular" + std::to_string(specularIdx++);
				break;
			case aiTextureType_NONE:
				name = textures[i].name;
				break;
			}

			shader.setInt(name, i);
			textures[i].bind();
		}

		shader.setInt("noTex", 0);
	}

	VAO.bind();
	VAO.draw(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, noInstances);
	ArrayObject::clear();

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup() { 
	VAO.generate();	
	VAO.bind();

	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].generate();
	VAO["EBO"].bind();
	VAO["EBO"].setData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);

	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].generate();
	VAO["VBO"].bind();
	VAO["VBO"].setData<Vertex>(vertices.size(), &vertices[0], GL_STATIC_DRAW);

	VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
	VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
	VAO["VBO"].setAttPointer<GLfloat>(2, 2, GL_FLOAT, 8, 6);

	VAO["VBO"].clear();

	ArrayObject::clear();
}

void Mesh::cleanup() {
	VAO.cleanup();

	for (Texture t : textures) {
		t.cleanup();
	}
}