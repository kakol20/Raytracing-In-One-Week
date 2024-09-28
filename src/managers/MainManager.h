#pragma once

#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_glfw.h"
#include "../../ext/imgui/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

class MainManager {
public:
	MainManager() {};
	~MainManager() {};

	enum class UpdateType {
		NONE, CONTINUE, BREAK
	};

	static int Init();
	static MainManager::UpdateType Update();
	static void Render();
	static void Shutdown();

	static void ErrorCallback(int error, const char* description);

private:
	static GLFWwindow* m_window;
	static unsigned int m_width, m_height;
};