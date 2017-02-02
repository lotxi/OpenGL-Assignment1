#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include "../glm/glm.hpp"
#include "Sweep.h"
#include <sstream>
#include <istream>
#include <vector>
#include "../glew/glew.h"	// include GL Extension Wrangler
#include <iterator>

class InputReader
{
public:
	InputReader(std::string file);
	~InputReader();
	/*std::vector<glm::vec3>* getProfileCurves();
	std::vector<glm::vec3>* getTrajectoryCurves();*/


	int getVerticesSize() const
	{
		return verticesSize;
	}

	int getIndicesSize() const
	{
		return indicesSize;
	}

	GLuint* getIndices();
	GLfloat* getVertices();
private:
	std::ifstream input;
	//Sweep readTranslationalSweep(std::ifstream & input);
	//Sweep readRotationalSweep(std::ifstream & input);
	std::vector<glm::vec3>* profileCurves;
	std::vector<glm::vec3>* trajectoryCurves;
	void readTranslationalSweep();
	void readRotationalSweep();
	glm::vec3 parseLine(std::string line) const;
	std::vector<glm::vec3>* storePoints(int points, std::ifstream & input);
	int verticesSize;
	int indicesSize;


	GLfloat *vertices;
	GLuint *indices;

};
