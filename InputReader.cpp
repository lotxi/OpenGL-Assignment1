#include "InputReader.h"

InputReader::InputReader(std::string file)
{
	input.open(file); 
	profileCurves = nullptr;
	trajectoryCurves = nullptr;
	if (input.is_open())
	{
		// File input stream loaded
		std::string line;

		getline(input, line);
		int mode = stoi(line);
		if (mode == 0) // Process translational sweep
		{
			readTranslationalSweep();
		}
		else {
			readRotationalSweep();
		}

	}
	else {
		throw "Error reading input file";
	}

}

//std::vector<glm::vec3>* InputReader::getProfileCurves()
//{
//	std::cout << "Returning profile curve" << std::endl;
//	return profileCurves;
//}

//std::vector<glm::vec3>* InputReader::getTrajectoryCurves()
//{
//	std::cout << "Returning trajectory curve" << std::endl;
//	return trajectoryCurves;
//}

GLuint* InputReader::getIndices()
{
	return indices;
}

GLfloat* InputReader::getVertices()
{
	return vertices;
}


int main1()
{
	InputReader* test = new InputReader("translational_shape.txt");
	// Store the original values of profile curve in vertices matrix
	delete(test);
	return 1;
	
}

std::vector<glm::vec3>* InputReader::storePoints(int points, std::ifstream & input)
{
	std::vector<glm::vec3> *result=new std::vector<glm::vec3>;
	std::string line;
	// Read and store profile curves
	for (int i = 0; i<points; i++)
	{
		getline(input, line);
		result->push_back(parseLine(line));
	}

	for (int j = 0; j<result->size(); j++)
	{
		std::cout << "x: " << (*result)[j].x << ", y: " << (*result)[j].y << ", z: " << (*result)[j].z << std::endl;
	}
	std::cout << std::endl;
	return result;
}

void InputReader::readTranslationalSweep()
{
	std::string line;
	std::cout << "Input is translational sweep" << std::endl;
	getline(input, line);
	int psize = std::stoi(line);
	std::cout << "Number of points in Profile Curve: " << psize << std::endl;
	profileCurves = storePoints(psize, input);

	getline(input, line);
	int tsize = stoi(line);
	std::cout << "Number of Points in Trajectory Curve: " << tsize << std::endl;

	trajectoryCurves = storePoints(tsize, input);

	int pSize = profileCurves->size();
	int tSize = trajectoryCurves->size();

	std::cout << "Psize: " << pSize << std::endl;
	std::cout << "Tsize: " << tSize << std::endl;

	//std::vector<glm::vec3> vContainer;
	verticesSize = pSize * tSize * 6;
	indicesSize = (pSize - 1)*(tSize - 1) * 6;
	indices = new GLuint[indicesSize];
	vertices = new GLfloat[verticesSize];
	int indexIndices = 0;
	int verticesIndex = 0; // K is used to track indices in the vertices array

	for (int i = 0; i<pSize; i++)
	{
		// Copy the initial profile curve into the vertices array
		vertices[verticesIndex++] = (*profileCurves)[i].x;
		vertices[verticesIndex++] = (*profileCurves)[i].y;
		vertices[verticesIndex++] = (*profileCurves)[i].z;
		vertices[verticesIndex++] = 0;
		vertices[verticesIndex++] = 0;
		vertices[verticesIndex++] = (*profileCurves)[i].z;
	}

	for (int i = 0; i < tSize - 1; i++)
	{
		for (int j = 0; j < pSize; j++)
		{
			// Translate each vertex from the previous iteration of P by the matching value in vector (t_(i+1)-t_(i))
			glm::vec4 temp = glm::vec4(vertices[verticesIndex - 6 * pSize], vertices[verticesIndex - 6 * pSize + 1],
				vertices[verticesIndex - 6 * pSize + 2], 1.0f);
			glm::mat4 translation = glm::translate(glm::mat4(), (*trajectoryCurves)[i + 1] - (*trajectoryCurves)[i]);
			temp = translation * temp;

			// Copy the translated vertices to vertices array
			vertices[verticesIndex++] = temp.x;
			vertices[verticesIndex++] = temp.y;
			vertices[verticesIndex++] = temp.z;
			vertices[verticesIndex++] = 0;
			vertices[verticesIndex++] = 0;
			vertices[verticesIndex++] = temp.z;

			// Calculate the indices of the vertices that will form triangles
			if (j != pSize - 1)
			{
				// First triangle
				indices[indexIndices] = verticesIndex / 6 - 1;
				indices[indexIndices + 1] = verticesIndex / 6;
				indices[indexIndices + 2] = verticesIndex / 6 - pSize;
				// Second triangle
				indices[indexIndices + 3] = verticesIndex / 6 - 1;
				indices[indexIndices + 4] = verticesIndex / 6 - pSize;
				indices[indexIndices + 5] = verticesIndex / 6 - pSize - 1;

				indexIndices += 6;
			}
		}
	}


}

void InputReader::readRotationalSweep()
{
	std::string line;
	std::cout << "Input is rotational sweep" << std::endl;
	getline(input, line);
	int spans = stoi(line);
	std::cout << "Number of spans: " << spans << std::endl;
	getline(input, line);
	int pSize = stoi(line);
	std::cout << "Number of points in Profile Curve: " << pSize << std::endl;
	profileCurves = storePoints(pSize, input);

	float rotationAngle = glm::radians(360.0f / float(spans));

	verticesSize = pSize * spans * 6;
	indicesSize = (pSize - 1) * spans * 6;
	int verticesIndex = 0;
	int indicesIndex = 0;
	vertices = new GLfloat[verticesSize];
	indices = new GLuint[indicesSize];

	for (int s = 0; s<spans; s++) // For every span
	{
		for (int p=0; p < pSize; p++) // For every point in the given profile curve
		{
			glm::vec4 point = glm::vec4((*profileCurves)[0], 1.0f); // Create a vec4 from the point
			glm::mat4 rotation = glm::mat4(1.0f);
			point = point * glm::rotate(rotation, rotationAngle * s, glm::vec3(0, 0, 1));
			
			vertices[verticesIndex++] = point.x;
			vertices[verticesIndex++] = point.y;
			vertices[verticesIndex++] = point.z;
			vertices[verticesIndex++] = 0;
			vertices[verticesIndex++] = 0;
			vertices[verticesIndex++] = point.z;

			if (p>0)
			{
				if (s>0)
				{
					indices[indicesIndex] = verticesIndex / 6 - 1;
					indices[indicesIndex+1] = verticesIndex / 6 - 1 - 1;
					indices[indicesIndex+2] = verticesIndex / 6 - 1 - pSize - 1;
					indicesIndex += 3;

					indices[indicesIndex] = verticesIndex / 6 - 1;
					indices[indicesIndex + 1] = verticesIndex / 6 - pSize - 1;
					indices[indicesIndex + 2] = verticesIndex / 6 - 1 - pSize - 1;
					indicesIndex += 3;


				}
				else
				{
					//first triangle
					indices[indicesIndex] = verticesIndex / 6 - 1;
					indices[indicesIndex] = verticesIndex / 6 - 1 - 1;
					indices[indicesIndex] = (verticesSize / 6) - 1 - pSize + (verticesIndex / 6) - 1;
					indicesIndex += 3;

					//second triangle
					indices[indicesIndex] = verticesIndex / 6 - 1;
					indices[indicesIndex] = verticesSize / 6 - 1 - pSize + verticesIndex / 6 -1;
					indices[indicesIndex] = verticesSize / 6 - pSize + verticesIndex / 6 - 1 - 1;
					indicesIndex += 3;
					
				}
			}

			
		}
	}

	
}

glm::vec3 InputReader::parseLine(std::string line) const
{
	std::stringstream ss(line);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vstrings(begin, end);

	float x, y, z;
	x = stof(vstrings[0]);
	y = stof(vstrings[1]);
	z = stof(vstrings[2]);

	return glm::vec3(x, y, z);
}

InputReader::~InputReader()
{
	//std::cout << "Destructor for InputReader has been called" << std::endl;
	//if (profileCurves!=nullptr)
	//{
	//	delete profileCurves;
	//}
	//if (trajectoryCurves != nullptr)
	//{
	//	delete trajectoryCurves;
	//}
}

