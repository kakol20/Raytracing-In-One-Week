#include "oof/oof.h"

#include "FastWrite.h"
#include "Random.h"
#include "String.h"

#include "Raytracing.h"

Raytracing::Raytracing() {
	// ----- SCENE SETTINGS -----
	m_debugScene = false;

	// ----- RENDER SETTINGS -----
	m_aperture = 0.1f;
	m_imageHeight = 720;
	m_imageWidth = 1280;
	m_rayDepth = 12;
	m_shadowDepth = 4;

	m_renderMode = "color";
	if (m_renderMode == "normal") {
		m_renderNormals = true;
		m_renderAlbedo = false;
	}
	else if (m_renderMode == "albedo") {
		m_renderNormals = false;
		m_renderAlbedo = true;
	}
	else if (m_renderMode == "color" || m_renderMode == "colour") {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}
	else {
		m_renderNormals = false;
		m_renderAlbedo = false;
	}

	m_renderScene = "final";

	m_samplesPerPixel = 128;
	m_tileSize = 32;
	m_verticalFOV = 19.8f;

	m_tilesRendered = 0;
	m_nextAvailable = 0;

	m_useThreads = 6;

	m_nearZero = 1e-8f;
}

bool Raytracing::Init() {
	Random::Seed = 2790598843;

	// ----- SETTINGS FILE -----
	std::fstream settings;
	String consoleOutput;
	consoleOutput += oof::reset_formatting();

	settings.open("settings.cfg", std::ios_base::in);
	if (settings.is_open()) {
		//std::cout << "----- SETTINGS -----\n";
		while (!settings.eof()) {
			String line;
			settings >> line;

			consoleOutput += line.GetChar();
			consoleOutput += '\n';

			String first = line.GetFirst("=");
			//const char* second = line.GetSecond("=");

			if (first == "imageWidth") {
				m_imageWidth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "imageHeight") {
				m_imageHeight = String::ToInt(line.GetSecond("="));
			}
			else if (first == "shadowDepth") {
				m_shadowDepth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "rayDepth") {
				m_rayDepth = String::ToInt(line.GetSecond("="));
			}
			else if (first == "renderMode") {
				m_renderMode = line.GetSecond("=");
			}
			else if (first == "scene") {
				m_renderScene = line.GetSecond("=");
			}
			else if (first == "samples") {
				m_samplesPerPixel = String::ToInt(line.GetSecond("="));
			}
			else if (first == "threads") {
				m_useThreads = (unsigned int)String::ToInt(line.GetSecond("="));

				m_useThreads = m_useThreads <= 0 ? std::thread::hardware_concurrency() : m_useThreads;
			}
			else if (first == "tileSize") {
				m_tileSize = String::ToInt(line.GetSecond("="));
			}
			else if (first == "aperture") {
				m_aperture = String::ToFloat(line.GetSecond("="));
			}
			else if (first == "verticalFOV") {
				m_verticalFOV = String::ToFloat(line.GetSecond("="));
			}
			else if (first == "randomSeed") {
				Random::Seed = String::ToInt(line.GetSecond("="));
			}
		}

		settings.close();
	}
	else {
		// settings file not found
		settings.open("settings.cfg", std::ios_base::out);

		constexpr unsigned int maxSeed = (std::numeric_limits<unsigned int>::max)();

		settings << "# Image Settings\n"
			<< "imageWidth=" << m_imageWidth << '\n'
			<< "imageHeight=" << m_imageHeight << "\n#\n"
			<< "# Render Settings\n"
			<< "rayDepth=" << m_rayDepth << '\n'
			<< "shadowDepth=" << m_shadowDepth << '\n'
			<< "threads=" << m_useThreads << '\n'
			<< "samplesPerPixel=" << m_samplesPerPixel << '\n'
			<< "tileSize=" << m_tileSize << '\n'
			<< "## color, normal, albedo or all\n"
			<< "renderMode=" << m_renderMode << '\n'
			<< "## final, textured or debug\n"
			<< "scene=" << m_renderScene << "\n#\n"
			<< "# Camera Settings\n"
			<< "aperture=" << m_aperture << '\n'
			<< "## In degrees\n"
			<< "verticalFOV=" << m_verticalFOV << "\n#\n"
			<< "# Max Seed - " << maxSeed << '\n'
			<< "randomSeed=" << Random::Seed;

		settings.close();
	}

	// fix zero seed
	if (Random::Seed == 0) {
		Random::Seed = 1 | ((unsigned int)0b1 << 31);
	}

	FastWrite::Write(consoleOutput);
	system("pause");
	m_hdri.Read("images/hdri/spruit_sunrise_2k.png", Image::ColorMode::sRGB);

	return true;
}

bool Raytracing::Run() {
	String output("");
	output += '\a';

	FastWrite::Write(output);

	m_hdri.Write("temp/test.png", Image::ColorMode::sRGB);

	return true;
}
