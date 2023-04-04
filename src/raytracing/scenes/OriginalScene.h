#pragma once

#include "Scene.h"

class OriginalScene : public Scene {
public:
	OriginalScene();
	virtual ~OriginalScene();

	virtual void Create(Settings& settings);


private:
	Float m_bgStrength;

private:

	virtual Vector3D BackgroundRay(const Vector3D& unitDir);
};