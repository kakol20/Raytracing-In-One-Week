#define TEST

#include <iomanip>
#include <iostream>
#include <limits>
#include <stdint.h>

#include "misc/Random.h"
#include "wrapper/Float.h"
#include "maths/Vector3D.h"

//thread_local Random::SeedType Seed = 0xACE1u;
#ifdef WIN32
thread_local uint32_t Random::Seed = 0xACE1u;
#else
thread_local uint64_t Random::Seed = 0xACE1u;
#endif // WIN32

const Float Float::PI = 3.1415926535897932384626433832795;
const Float Float::TAU = Float::PI * 2;
const Float Float::ToRadians = Float::PI / 180;

const Float Float::MaxVal = std::numeric_limits<Float::ForD>::max();
const Float Float::MinVal = std::numeric_limits<Float::ForD>::min();

const Vector3D Vector3D::Forward = Vector3D(0, 0, -1);
const Vector3D Vector3D::One = Vector3D(1, true);
const Vector3D Vector3D::Right = Vector3D(1, 0, 0);
const Vector3D Vector3D::Up = Vector3D(0, 1, 0);
const Vector3D Vector3D::Zero = Vector3D(0, 0, 0);

int main() {
	std::cout << std::setprecision(10);

#ifdef TEST
	std::cout << "Test\n\a";
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

	std::cin.ignore();
#endif // TEST

	return 0;
}