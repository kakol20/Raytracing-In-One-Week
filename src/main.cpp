#define OLC_PGE_APPLICATION

#include "../ext/olcPixelGameEngine.h"

#include "render/Render.h"

int main() {
	Render render;
	if (render.Construct(1280, 720, 1, 1, false, true)) render.Start();

	return 0;
}