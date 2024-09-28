#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

#include "../../ext/imgui/imgui.h"
#include "../../ext/imgui/imgui_impl_glfw.h"
#include "../../ext/imgui/imgui_impl_opengl3.h"

#include "../image/Image.h"


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

	static Image m_image;

	static std::mutex m_mtx;
	static std::vector<std::thread> m_threads;
	static std::map<std::thread::id, bool> m_threadStatus;
	static void ProcessPixels(const int minY, const int maxY);

	static bool m_allFinished;
};