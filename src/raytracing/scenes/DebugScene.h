#pragma once

#include "Scene.h"

class DebugScene : public Scene {
public:
	DebugScene();
	virtual ~DebugScene();

	virtual void Create(Settings& settings);

private:
	virtual Vector3D BackgroundRay(const Vector3D& unitDir);
};