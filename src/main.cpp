#define OOF_IMPL
//#define TEST

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdint.h>

#include "../ext/oof/oof.h"

#include "maths/Vector3D.h"
#include "misc/Random.h"
#include "wrapper/Fastwrite.h"
#include "wrapper/Float.h"
#include "wrapper/Image.h"
#include "raytracing/Raytracing.h"

Raytracing RT;

int main() {
	FastWrite::EnableVTMode();

	std::cout << std::setprecision(6);

	FastWrite::Reset();

#ifdef TEST
	//std::cout << "Test\n\a";
	std::cout << "PI: " << Float::PI << '\n';
	std::cout << "TAU: " << Float::TAU << '\n';
	std::cout << "To Radians: " << Float::ToRadians << '\n';
	std::cout << "Min Value: " << Float::MinVal << '\n';
	std::cout << "Max Value: " << Float::MaxVal << '\n';

#ifdef WIN32
	std::cout << "Sin Close to Zero: " << Float::Sin(1e-4) << '\n';
#else
	std::cout << "Sin Close to Zero: " << Float::Sin(1e-8) << '\n';
#endif // WIN32

	std::cout << "Random Float: " << Random::RandomFloat() << '\n';
	std::cout << "Sizeof Float: " << sizeof(Float) << " vs Sizeof double: " << sizeof(double) << '\n';
	std::cout << "Sizeof Vector3D: " << sizeof(Vector3D) << '\n';
	std::cout << "PI to String: " << Float::ToString(Float::PI) << '\n';

	std::cin.ignore();

#else
	FastWrite::Write("Press enter to start\n");
	std::cin.ignore();

	FastWrite::Write("Initializing\n");

	if (!RT.Init()) {
		FastWrite::Write("Failed to initialize raytracer\n");
		//std::clog << "Failed to initialize raytracer\n";

		std::cin.ignore();
		return -1;
	}

	FastWrite::Write("\nPress enter to run\n");
	std::cin.ignore();

	if (!RT.Run()) {
		FastWrite::Write("Failed to run raytracer\n");
		//std::clog << "Failed to initialize raytracer\n";

		std::cin.ignore();
		return -1;
	}
#endif // TEST

	return 0;
}