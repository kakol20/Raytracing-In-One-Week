#include <iostream>

#include "MainManager.h"

GLFWwindow* MainManager::m_window = nullptr;

unsigned int MainManager::m_width = 800;
unsigned int MainManager::m_height = 800;
Image MainManager::m_image = Image();

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

	if (!m_image.Read("data/suzanne.png", 3)) return EXIT_FAILURE;
	m_image.CreateTexture();

	return EXIT_SUCCESS;
}

MainManager::UpdateType MainManager::Update() {
	if (glfwWindowShouldClose(m_window)) return MainManager::UpdateType::BREAK;

	glfwPollEvents();
	if (glfwGetWindowAttrib(m_window, GLFW_ICONIFIED) != 0) {
		ImGui_ImplGlfw_Sleep(10);
		return MainManager::UpdateType::CONTINUE;
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

	/*int imageWidth, imageHeight;
	GLuint imageTexture;
	m_image.ToImGui(imageTexture, imageWidth, imageHeight);*/

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