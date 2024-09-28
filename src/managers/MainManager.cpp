#include <iostream>
#include <chrono>
#include "../other/Log.h"

#include "MainManager.h"

GLFWwindow* MainManager::m_window = nullptr;

unsigned int MainManager::m_width = 800;
unsigned int MainManager::m_height = 800;
Image MainManager::m_image;
bool MainManager::m_allFinished = false;
std::mutex MainManager::m_mtx;
std::vector<std::thread> MainManager::m_threads;
std::map<std::thread::id, bool> MainManager::m_threadStatus;

int MainManager::Init() {
	glfwSetErrorCallback(MainManager::ErrorCallback);

	if (!glfwInit()) return EXIT_FAILURE;

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	m_window = glfwCreateWindow(m_width, m_height, "Raytracing", nullptr, nullptr);
	if (m_window == nullptr) return EXIT_FAILURE;
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();/* (void)io;*/
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	glfwMaximizeWindow(m_window);

	// ----- OTHER -----

	//if (!m_image.Read("data/suzanne.png", 3)) return EXIT_FAILURE;
	m_image = Image(800, 800, 3);
	m_image.CreateTexture();

	const int threadCount = 4;
	const int yDelta = m_image.GetHeight() / 4;
	for (int i = 0; i < threadCount; i++) {
		const int minY = i * yDelta;
		const int maxY = (i + 1) >= threadCount ? m_image.GetHeight() : (i + 1) * yDelta;

		m_threads.push_back(std::thread(MainManager::ProcessPixels, minY, maxY));
		m_threads.back().detach();
		m_threadStatus[m_threads.back().get_id()] = false;
	}

	Log::StartTime();

	return EXIT_SUCCESS;
}

MainManager::UpdateType MainManager::Update() {
	if (glfwWindowShouldClose(m_window)) return MainManager::UpdateType::BREAK;

	glfwPollEvents();
	if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
		ImGui_ImplGlfw_Sleep(10);
		return MainManager::UpdateType::CONTINUE;
	}

	// -- Check if threads finished --
	if (!m_allFinished) {
		m_allFinished = true;
		for (auto it = m_threadStatus.begin(); it != m_threadStatus.end(); it++) {
			if (!it->second) m_allFinished = false;
		}

		if (!m_allFinished) {
			// -- Update every 1 second --
			if (Log::CheckTimeSeconds(1.)) {
				m_mtx.lock();
				m_image.UpdateTexture();
				m_mtx.unlock();

				Log::StartTime();
			}
		} else {
			for (auto it = m_threads.begin(); it != m_threads.end(); it++) {
				it->join();
			}

			m_image.UpdateTexture();
		}
	}

	return MainManager::UpdateType::NONE;
}

void MainManager::Render() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// ----- SHOW IMAGE -----

	// https://cplusplus.com/reference/cstdio/printf/
	//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Appearing);

	ImGui::Begin("Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	m_image.RenderImage();
	ImGui::End();

	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(m_window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	const float col = 28.f / 255.f;
	glClearColor(col, col, col, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);
}

void MainManager::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void MainManager::ErrorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void MainManager::ProcessPixels(const int minY, const int maxY) {
	using namespace std::chrono_literals;
	for (int y = minY; y < maxY; y++) {
		for (int x = 0; x < m_image.GetWidth(); x++) {
			double rValueD = (double)x / double(m_image.GetWidth());
			double gValueD = (double)y / double(m_image.GetHeight());

			rValueD = std::pow(rValueD, 1. / 2.4);
			gValueD = std::pow(gValueD, 1. / 2.4);

			const uint8_t rValueU = (uint8_t)std::round(rValueD * 255.);
			const uint8_t gValueU = (uint8_t)std::round(gValueD * 255.);

			const size_t index = m_image.GetIndex(x, y);

			m_image.SetData(index + 0, rValueU);
			m_image.SetData(index + 1, gValueU);
			m_image.SetData(index + 2, 0);

		}
		// artificial delay
		std::this_thread::sleep_for(1ms);
	}
	m_threadStatus[std::this_thread::get_id()] = true;
}
