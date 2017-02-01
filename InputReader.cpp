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

std::vector<glm::vec3>* InputReader::getProfileCurves()
{
	std::cout << "Returning profile curve" << std::endl;
	return profileCurves;
}

std::vector<glm::vec3>* InputReader::getTrajectoryCurves()
{
	std::cout << "Returning trajectory curve" << std::endl;
	return trajectoryCurves;
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
	std::cout << "Creating trajectory curve from rotations..." << std::endl;
	float rotationAngle = glm::radians(360.0f / float(spans));
	trajectoryCurves = new std::vector<glm::vec3>;

	int k = 0; // Tracks the indices of vertices as we enter them into the array
	for (int s = 0; s<spans; s++)
	{


		// The point to be rotated
		glm::vec4 point = glm::vec4((*profileCurves)[0], 1.0f);

		glm::mat4 rotationMatrix = glm::mat4(1.0f);
		point = point * glm::rotate(rotationMatrix, rotationAngle * s, glm::vec3(0, 0, 0));
		std::cout << point.x << ", " << point.y << ", " << point.z << std::endl;
		trajectoryCurves->push_back(glm::vec3(point.x, point.y, point.z));

	}

	
}

glm::vec3 InputReader::parseLine(std::string line)
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

