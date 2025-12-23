#ifndef STB_IMAGE_H
#define STB_IMAGE_H

// Only define the implementation once in your entire project
// This should be defined in ONE .cpp file (typically material.cpp)
#ifdef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif

// Include the actual STB image library
#include "stb_image.h"
#include "config.h"

#endif // STB_IMAGE_H


std::vector<std::string> split(std::string line, std::string delimiter)
{
	std::vector<std::string> splitLine;
	size_t pos = 0;
	std::string token;
	while ((pos = line.find(delimiter)) != std::string::npos)
	{
		token = line.substr(0, pos);
		splitLine.push_back(token);
		line.erase(0, pos + delimiter.size());
	}
	// here there is no delimiter to signal token as this is last words.
	splitLine.push_back(line);
	return splitLine;	
}
