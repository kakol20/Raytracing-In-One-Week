#pragma once

#include "Scene.h"

class FullScene : public Scene {
public:
	FullScene();
	virtual ~FullScene();

	virtual void Create(Settings& settings);

private:
	virtual Vector3D BackgroundRay(const Vector3D& unitDir);
};