#pragma once
#include <fstream>
#include <map>
#include <thread>
#include <vector>

#include "Image.h"
#include "String.h"

class Raytracing {
public:
	Raytracing();
	~Raytracing() {};

	bool Init();
	bool Run();

private:
	Image m_hdri;
	Image m_render;

	bool m_debugScene;
	bool m_renderAlbedo;
	bool m_renderNormals;
	float m_aperture, m_verticalFOV;
	float m_nearZero;
	int m_imageWidth, m_imageHeight, m_samplesPerPixel, m_rayDepth, m_tileSize, m_shadowDepth;
	int m_tilesRendered;
	size_t m_nextAvailable;
	size_t m_useThreads;
	std::fstream m_log;
	String m_renderMode;
	String m_renderScene;
};

