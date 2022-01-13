#include "Raytracing.h"
#include "LinearFeedbackShift.h"
#include <chrono>
#include <fstream>

Raytracing raytracing;

unsigned int LinearFeedbackShift::Seed = 13012022;

int main() {
	std::time_t current_time = time(0);
	//LinearFeedbackShift::Seed = static_cast<unsigned int>(current_time);

	raytracing.Init();

	auto begin = std::chrono::high_resolution_clock::now();

	if (!raytracing.Run()) return -1;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - begin;
	//float elapsedF = std::static_

	std::fstream runTime;
	runTime.open("images/runTime.txt", std::ios_base::out);

	if (runTime.is_open()) {
		runTime << "Elapsed Time: " << elapsed << '\n';
	}

	return 0;
}