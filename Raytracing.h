#pragma once
#include "Image.h"

class Raytracing {
public:
	Raytracing() {};
	~Raytracing() {};

	bool Init();
	bool Run();

private:
	Image m_hdri;
};

