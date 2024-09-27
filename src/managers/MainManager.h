#pragma once

#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_glfw.h"
#include "../../ext/imgui/imgui_impl_opengl3.h"

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h>


class MainManager {
public:
	MainManager() {};
	~MainManager() {};

	static int Init();
	static bool Update();
	static void Render();
	static void Shutdown();

	static void ErrorCallback(int error, const char* description);

private:
	static GLFWwindow* m_window;
	static unsigned int m_width, m_height;
};