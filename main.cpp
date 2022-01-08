#include "Raytracing.h"
#include <chrono>
#include <fstream>

Raytracing raytracing;

int main() {
	std::fstream runTime;
	runTime.open("images/runTime.txt", std::ios_base::out);

	auto begin = std::chrono::high_resolution_clock::now();

	if (!raytracing.Run()) return -1;

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - begin;
	//float elapsedF = std::static_

	if (runTime.is_open()) {
		runTime << "Elapsed Time: \n";
		runTime << "    " << elapsed;
	}

	return 0;
}