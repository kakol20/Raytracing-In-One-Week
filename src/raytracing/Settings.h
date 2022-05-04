#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>
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

	std::string& operator[](std::string first) { return m_settings[first]; };

private:
	std::map<std::string, std::string> m_settings;
	std::fstream m_file;
};
#endif // !SETTINGS_H
