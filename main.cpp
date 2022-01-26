#include <chrono>
#include <fstream>
#include <mutex>

#include "Raytracing.h"
#include "LinearFeedbackShift.h"
#include "String.h"
#include "StaticMutex.h"

unsigned int LinearFeedbackShift::Seed = 3881995897;
std::mutex StaticMutex::s_mtx = std::mutex();

bool RenderMode(const char* renderMode);

Raytracing raytracing;

int main() {
	raytracing.Init();

	String renderMode = "all";

	std::fstream settings;

	settings.open("settings.cfg", std::ios_base::in);

	if (settings.is_open()) {
		while (!settings.eof()) {
			String line;
			settings >> line;

			/*std::cout << line;
			std::cout << '\n';*/

			String first = line.GetFirst("=");

			if (first == "renderMode") {
				renderMode = line.GetSecond("=");

				break;
			}
		}
	}
	settings.close();

	//std::cout << "Render Mode: " << renderMode << '\n';
	
	system("pause");

	auto begin = std::chrono::high_resolution_clock::now();

	if (renderMode == "all") {
		std::cout << "\nRendering Albedo\n";
		if (!RenderMode("albedo")) return -1;

		std::cout << "\nRendering Normal\n";
		if (!RenderMode("normal")) return -1;

		std::cout << "\nRendering Color\n";
		if (!RenderMode("color")) return -1;
		std::cout << '\n';
	}
	else {
		if (!RenderMode(renderMode.GetChar())) return -1;
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::minutes>(end - begin);
	//float elapsedF = std::static_

	std::fstream runTime;
	runTime.open("runTime.txt", std::ios_base::out);

	if (runTime.is_open()) {
		runTime << "Elapsed Time: " << elapsed << '\n';
	}

	return 0;
}

bool RenderMode(const char* renderMode) {
	raytracing.SetRenderMode(renderMode);

	return raytracing.Run();
}
