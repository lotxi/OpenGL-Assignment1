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


GLuint* InputReader::getIndices()
{
	return indices;
}

GLfloat* InputReader::getVertices()
{
	return vertices;
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
		vertices[verticesIndex++] = 0.9f;
		vertices[verticesIndex++] = 1.0 - (*profileCurves)[i].z;
		vertices[verticesIndex++] = 0.0f;
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
			vertices[verticesIndex++] = 0.9f;
			vertices[verticesIndex++] = 1.0 - temp.z;
			vertices[verticesIndex++] = 0.0f;

			// Calculate the indices of the vertices that will form triangles
			if (j != pSize - 1) // Proceed only if this is not the bottom point in the profile curve
			{
				int bottomRight = verticesIndex / 6;
				int topRight = bottomRight - 1;
				int topLeft = topRight-pSize;
				int bottomLeft = bottomRight-pSize;
				// First triangle
				indices[indexIndices++] = bottomRight;
				indices[indexIndices++] = topRight;
				indices[indexIndices ++] = bottomLeft; 
				// Second triangle
				indices[indexIndices ++] = topRight;
				indices[indexIndices ++] = topLeft;
				indices[indexIndices ++] = bottomLeft;
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
	indicesSize = (pSize-1) * spans * 6;

	vertices = new GLfloat[verticesSize];
	indices = new GLuint[indicesSize];

	int verticesIndex = 0;
	int indicesIndex = 0;

	for (int s = 0; s<spans; s++) // For every span
	{
		for (int p=0; p < pSize; p++) // For every point in the given profile curve
		{
			glm::vec4 point = glm::vec4((*profileCurves)[p], 1); // Create a vec4 from the point
			//glm::mat4 rotation = glm::mat4(1.0f);
			point = point * glm::rotate(glm::mat4(1.0f), rotationAngle * s, glm::vec3(0, 0, 1));
			
			vertices[verticesIndex++] = point.x;
			vertices[verticesIndex++] = point.y;
			vertices[verticesIndex++] = point.z;
			vertices[verticesIndex++] = 0.9f;
			vertices[verticesIndex++] = 1.0-point.z;
			vertices[verticesIndex++] = 0.0f;


			if (p>0)
			{
				int topRight = verticesIndex / 6 - 1;
				int bottomRight = topRight - 1;

				if (s>0)
				{
					
					int topLeft = topRight - pSize;;
					int bottomLeft = topLeft - 1;
					// First triangle
					indices[indicesIndex++] = topRight;
					indices[indicesIndex++] = bottomRight;
					indices[indicesIndex++] = bottomLeft;

					indices[indicesIndex++] = topRight;
					indices[indicesIndex++] = bottomLeft;
					indices[indicesIndex++] = topLeft;
					


				}
				else // Generate indices for triangles between the initial curve and the final one
				{
					
					int topLeft = verticesSize / 6 - pSize + verticesIndex / 6 - 1;
					int bottomLeft = topLeft - 1;
					// First triangle
					indices[indicesIndex++] = topRight;
					indices[indicesIndex++] = bottomRight;
					indices[indicesIndex++] = bottomLeft;

					// Second triangle
					indices[indicesIndex++] = topRight;
					indices[indicesIndex++] = topLeft;
					indices[indicesIndex++] = bottomLeft;
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

}

