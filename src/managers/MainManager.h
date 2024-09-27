#pragma once

#include <imgui.h>

class MainManager {
public:
	MainManager() {};
	~MainManager() {};

	static void Init();
	static bool Update();
	static void Render();
	static void Shutdown();
};