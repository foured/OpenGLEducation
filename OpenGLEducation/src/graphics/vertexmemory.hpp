#ifndef VERTEXMEMORY_HPP
#define VERTEXMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

class BufferObject {
public:
	GLuint val;
	//type of  buffer
	GLenum type;

	BufferObject() {}
	BufferObject(GLenum type)
		: type(type) {}

	//generate
	void generate() {
		glGenBuffers(1, &val);
	}

	//bind object
	void bind() {
		glBindBuffer(type, val);
	}

	//set data
	template<typename T>
	void setData(GLuint noElements, T *data, GLenum usage) {
		glBufferData(type, noElements * sizeof(T), data, usage);
	}

	//update data
	template<typename T>
	void updateData(GLintptr offset, GLuint noElements, T *data) {
		glBufferSubData(type, offset, noElements * sizeof(T), data);
	}

	//set attrib pointers
	template<typename T>
	void setAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint devisor = 0) {
		glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
		glEnableVertexAttribArray(idx);
		if (devisor > 0) {
			//if instansing
			glVertexAttribDivisor(idx, devisor);
		}		
	}

	//clear buffers
	void clear() {
		glBindBuffer(type, 0);
	}

	//cleanup
	void cleanup() {
		glDeleteBuffers(1, &val);
	}
};

class ArrayObject {
public:
	GLuint val;

	std::map<const char*, BufferObject> buffers;

	//get buffer
	BufferObject& operator[](const char* key) {
		return buffers[key];
	}

	//generate
	void generate() {
		glGenVertexArrays(1, &val);
	}

	//bind object
	void bind() {
		glBindVertexArray(val);
	}

	void draw(GLenum mode, GLuint first, GLuint count) {
		glDrawArrays(mode, first, count);
	}

	//draw
	void draw(GLenum mode, GLsizei count, GLenum type, GLint indices, GLsizei instancecount = 1) {
		glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
	}

	//cleanup
	void cleanup() {
		glDeleteVertexArrays(1, &val);
		for (auto& pair : buffers) {
			pair.second.cleanup();
		}
	}

	//clear
	static void clear() {
		glBindVertexArray(0);
	}
};

#endif