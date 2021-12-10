#include "Image.h"

bool Image::PrintToConsole = true;

int main() {
	Image test(256, 256, 3);

	for (int x = 0; x <= 255; x++) {
		for (int y = 0; y <= 255; y++) {
			int index = test.GetIndex(x, y);

			test.SetData(index + 0, (uint8_t)x);
			test.SetData(index + 1, (uint8_t)y);
		}
	}

	test.Write("images/test.png");

	return 0;
}