#include "Raytracing.h"
#include "LinearFeedbackShift.h"
#include <chrono>
#include <fstream>

unsigned int LinearFeedbackShift::Seed = 18012022;

Raytracing raytracing;

int main() {
	raytracing.Init();

	auto begin = std::chrono::high_resolution_clock::now();

	if (!raytracing.Run()) return -1;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - begin;
	//float elapsedF = std::static_

	std::fstream runTime;
	runTime.open("runTime.txt", std::ios_base::out);

	if (runTime.is_open()) {
		runTime << "Elapsed Time: " << elapsed << '\n';
	}

	return 0;
}