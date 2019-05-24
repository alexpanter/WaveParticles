#pragma once

// STANDARD
#include <fstream>
#include <iostream>
#include <string>

// CHECK FOR WINDOWS
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif


namespace Core::FileIO
{
	// returns the extension for a target file string
	std::string getFileExtension(const std::string& fileName)
	{
		return fileName.substr(fileName.find_last_of(".") + 1);
	}

	// auxillary function to read from a file
	std::string readFileContents(const std::string& filePath)
	{
		std::string contents;
		std::ifstream fileStream(filePath, std::ios::in);

		if (!fileStream.is_open())
		{
			std::cerr << "Could not read file '"
				<< filePath << "'." << std::endl;
			return "";
		}

		std::string line = "";
		while (!fileStream.eof())
		{
			std::getline(fileStream, line);
			contents.append(line + "\n");
		}

		fileStream.close();
		return contents;
	}

	// intended functionality to retrieve platform-dependent
	// path separator
	//
	// TODO: Could be optimized by placing this functionality inside Platform macros
	inline char getPlatformSeparator()
	{
#ifdef WIN32
		return '\\';
#else
		return '/';
#endif
	}

	// creating a platform-dependent string path from the
	// specified target file
	std::string getPlatformFilePath(const char* file)
	{
		char sep = getPlatformSeparator();

		std::string filepath;

		for (const char* ptr = file; *ptr != '\0'; ptr++)
		{
			if (*ptr == '|')
			{
				filepath += sep;
			}
			else
			{
				filepath += *ptr;
			}
		}

		return filepath;
	}

	// almost same as above, e.g. usage:
	// getPlatformPath("path1/path2/path3") -> "path1/path2/path3/"
	std::string getPlatformPath(const char* path)
	{
		// TODO: Check if last char is already path-separator before concatenating!
		return getPlatformFilePath(path) + getPlatformSeparator();
	}
}