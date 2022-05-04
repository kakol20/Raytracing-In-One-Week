#define TEST

#include <iomanip>
#include <iostream>
#include <limits>

#include "wrapper/Float.h"

const Float Float::PI = 3.1415926535897932384626433832795;
const Float Float::TAU = Float::PI * 2;
const Float Float::ToRadians = Float::PI / 180;

const Float Float::MaxVal = std::numeric_limits<Float::ForD>::max();
const Float Float::MinVal = std::numeric_limits<Float::ForD>::min();

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

	std::cin.ignore();
#endif // TEST

	return 0;
}