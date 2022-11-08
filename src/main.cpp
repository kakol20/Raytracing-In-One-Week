#define OLC_PGE_APPLICATION

#include "../ext/olcPixelGameEngine.h"

#include "render/Render.h"

int main() {
	Render render;
	if (render.Construct(256, 256, 3, 3, false, true)) render.Start();

	return 0;
}