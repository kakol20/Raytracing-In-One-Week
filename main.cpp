#include <Windows.h>
#include <iostream>

#define OOF_IMPL
#include "oof/oof.h"

#include "Random.h"
#include "Raytracing.h"
#include "StaticMutex.h"
//#include "FastWrite.h"
//#include "String.h"

auto enable_vt_mode() -> void;

bool Image::PrintToConsole = true;
std::mutex StaticMutex::s_mtx = std::mutex();
thread_local String StaticMutex::s_output = "";
thread_local unsigned int Random::Seed = 1;

Raytracing RT;

int main() {
	enable_vt_mode();

	std::cout << oof::cursor_visibility(false) << oof::reset_formatting() << oof::clear_screen() << oof::bg_color({ 12, 12, 12 });

	if (!RT.Init()) {
		std::cout << oof::clear_screen() << oof::position(0, 0) << "Failed to initialize raytracer\n";

		return -1;
	}
	else {
		if (!RT.Run()) {
			std::cout << oof::clear_screen() << oof::position(0, 0) << "Failed to run raytracer\n";
			return -1;
		}
	}

	return 0;
}

auto enable_vt_mode() -> void {
	HANDLE const handle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (handle == INVALID_HANDLE_VALUE) {
		std::terminate(); // error handling
	}

	DWORD dwMode = 0;
	if (!GetConsoleMode(handle, &dwMode)) {
		std::terminate(); // error handling
	}

	if (dwMode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) {
		// VT mode is already enabled
		return;
	}

	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	if (!SetConsoleMode(handle, dwMode)) {
		std::terminate(); // error handling
	}
}