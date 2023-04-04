#include <sstream>
#include <algorithm>

#include "Settings.h"

Settings::Settings() {
	m_settings["imageWidth"] = "1280";
	m_settings["imageHeight"] = "720";
	m_settings["renderScale"] = "1.25";

	m_settings["rayDepth"] = "8";
	m_settings["threads"] = "12";
	m_settings["maxSamples"] = "128";
	m_settings["minSamples"] = "8";
	m_settings["noiseThreshold"] = "0.01";
	m_settings["tileSize"] = "32";
	m_settings["renderMode"] = "color";
	m_settings["scene"] = "original";

	m_settings["blurStrength"] = "0.1";
	m_settings["verticalFOV"] = "20";

	m_settings["randomSeed"] = "28042022";
}

Settings::~Settings() {
}

bool Settings::Read(const std::string& fileLocation) {
	m_file.open(fileLocation, std::ios_base::in);

	if (m_file.is_open()) {
		std::string line;
		bool isEmpty = true;
		while (getline(m_file, line)) {
			if (!(line[0] == '#' || line.empty())) {
				std::replace(line.begin(), line.end(), '=', ' ');

				std::stringstream ss(line);

				std::string name;
				std::string value;

				ss >> name;
				ss >> value;

				m_settings[name] = value;

				isEmpty = false;
			}
		}
		m_file.close();
		return !isEmpty;
	}
	m_file.close();

	return false;
}

void Settings::Write(const std::string& fileLocation) {
	m_file.open(fileLocation, std::ios_base::out);

	m_file << "# ----- IMAGE SETTINGS -----\n";
	m_file << "imageWidth=" << m_settings["imageWidth"] << '\n';
	m_file << "imageHeight=" << m_settings["imageHeight"] << '\n';
	m_file << "renderScale=" << m_settings["renderScale"] << '\n';

	m_file << "\n# ----- RENDER SETTINGS -----\n";
	m_file << "rayDepth=" << m_settings["rayDepth"] << '\n';
	m_file << "threads=" << m_settings["threads"] << '\n';
	m_file << "maxSamples=" << m_settings["maxSamples"] << '\n';
	m_file << "minSamples=" << m_settings["minSamples"] << '\n';
	m_file << "noiseThreshold=" << m_settings["noiseThreshold"] << '\n';
	m_file << "tileSize=" << m_settings["tileSize"] << '\n';

	m_file << "# color, normal, albedo, emission or all\n";
	m_file << "renderMode=" << m_settings["renderMode"] << '\n';

	m_file << "# original, debug or full\n";
	m_file << "scene=" << m_settings["scene"] << '\n';

	m_file << "\n# ----- CAMERA SETTINGS -----\n";
	m_file << "blurStrength=" << m_settings["blurStrength"] << '\n';
	m_file << "# in degrees - max 179\n";
	m_file << "verticalFOV=" << m_settings["verticalFOV"] << '\n';

	m_file << "\n# ----- MISC SETTINGS -----\n";
	m_file << "randomSeed=" << m_settings["randomSeed"];

	m_file.close();
}