#pragma once
#ifndef MESH_HPP
#define MESH_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <set>  
#include <stack>
#include <functional>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <glerror.hpp>

class Mesh {
public:

	Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals);

	GLuint getVertexBuffer();
	GLuint getUvBuffer();
	GLuint getNormalBuffer();
	GLuint getElementBuffer();
	std::vector<glm::vec3> * getIndexedVertices();
	std::vector<glm::vec2> * getIndexedUvs();
	std::vector<glm::vec3> * getIndexedNormals();
	std::vector<unsigned short> * getIndices();

	void attributeBuffer();
	void cleanup();
	void bind();

private:
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	std::vector<unsigned short> indices;

	GLuint vertexbuffer;
	GLuint uvbuffer;
	GLuint normalbuffer;
	GLuint elementbuffer;
};

#endif
