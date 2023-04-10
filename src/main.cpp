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

#ifdef WIN32
const Float::ForD Float::NearZero = 1e-4f;
#else
const Float::ForD Float::NearZero = 1e-6;
//const Float::ForD Float::NearZero = 1e-10;
#endif // WIN32

const Vector3D Vector3D::Forward = Vector3D(0, 0, -1);
const Vector3D Vector3D::One = Vector3D(1, true);
const Vector3D Vector3D::Right = Vector3D(1, 0, 0);
const Vector3D Vector3D::Up = Vector3D(0, 1, 0);
const Vector3D Vector3D::Zero = Vector3D(0, true);

// private variable
const unsigned int Image::Threshold[] = {
	  0, 192,  48, 240,  12, 204,  60, 252,   3, 195,  51, 243,  15, 207,  63, 255,
	128,  64, 176, 112, 140,  76, 188, 124, 131,  67, 179, 115, 143,  79, 191, 127,
	 32, 224,  16, 208,  44, 236,  28, 220,  35, 227,  19, 211,  47, 239,  31, 223,
	160,  96, 144,  80, 172, 108, 156,  92, 163,  99, 147,  83, 175, 111, 159,  95,
	  8, 200,  56, 248,   4, 196,  52, 244,  11, 203,  59, 251,   7, 199,  55, 247,
	136,  72, 184, 120, 132,  68, 180, 116, 139,  75, 187, 123, 135,  71, 183, 119,
	 40, 232,  24, 216,  36, 228,  20, 212,  43, 235,  27, 219,  39, 231,  23, 215,
	168, 104, 152,  88, 164, 100, 148,  84, 171, 107, 155,  91, 167, 103, 151,  87,
	  2, 194,  50, 242,  14, 206,  62, 254,   1, 193,  49, 241,  13, 205,  61, 253,
	130,  66, 178, 114, 142,  78, 190, 126, 129,  65, 177, 113, 141,  77, 189, 125,
	 34, 226,  18, 210,  46, 238,  30, 222,  33, 225,  17, 209,  45, 237,  29, 221,
	162,  98, 146,  82, 174, 110, 158,  94, 161,  97, 145,  81, 173, 109, 157,  93,
	 10, 202,  58, 250,   6, 198,  54, 246,   9, 201,  57, 249,   5, 197,  53, 245,
	138,  74, 186, 122, 134,  70, 182, 118, 137,  73, 185, 121, 133,  69, 181, 117,
	 42, 234,  26, 218,  38, 230,  22, 214,  41, 233,  25, 217,  37, 229,  21, 213,
	170, 106, 154,  90, 166, 102, 150,  86, 169, 105, 153,  89, 165, 101, 149,  85
};

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