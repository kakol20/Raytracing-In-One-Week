#include "Image.h"
#include "Vector3D.h"

bool Image::PrintToConsole = true;

int main() {
	// Test image
	Image test(256, 256, 3);

	for (int x = 0; x <= 255; x++) {
		for (int y = 0; y <= 255; y++) {
			int index = test.GetIndex(x, y);

			test.SetData(index + 0, (uint8_t)x);
			test.SetData(index + 1, (uint8_t)y);
		}
	}

	test.Write("images/test.png");

	// Test Vector3D
	Vector3D vector1(1.0f, 2.0f, 3.0f);
	Vector3D vector2(4.0f, 5.0f, 6.0f);

	Vector3D division = vector1 / vector2;
	Vector3D divisionScale = vector1 / 7.0f;
	Vector3D multiplication = vector1 * vector2;
	Vector3D multiplicationScale = vector1 * 7.0f;
	Vector3D addition = vector1 + vector2;

	return 0;
}