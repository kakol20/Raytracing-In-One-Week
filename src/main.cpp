#include <iostream>

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>

#include "render/PixelRender.h"
#include "utility/Color.h"
#include "utility/Random.h"

#ifdef WIN32
thread_local uint32_t Random::Seed = 0xACE1u;
#else
thread_local uint64_t Random::Seed = 0xACE1u;
#endif // WIN32

#ifdef _DEBUG
int main() {
#else
int WinMain() {
#endif // _DEBUG

	PixelRender mainWindow(1280, 720, "Raytracing");
	if (!mainWindow.Init()) {
		// error initialising
	}

	while (mainWindow.Draw());

	return 0;
}