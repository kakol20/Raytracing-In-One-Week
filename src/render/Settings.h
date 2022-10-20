#pragma once

//#include <map>
#include <unordered_map>
#include <string>
#include <fstream>

class Settings {
public:
	Settings();
	~Settings();

	bool Read(const std::string& fileLocation);

	/// <summary>
	/// Creates new settings file with default values defined in constructor
	/// </summary>
	/// <param name="fileLocation"></param>
	void Write(const std::string& fileLocation);

	std::string& operator[](std::string first);

private:
	std::unordered_map<std::string, std::string> m_settings;
	std::fstream m_file;
};