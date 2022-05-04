#include <iostream>
#include <Windows.h>

#include "../../ext/oof/oof.h"

#include "FastWrite.h"

auto FastWrite::EnableVTMode() -> void {
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

void FastWrite::Reset() {
	std::string output = "";
	output += oof::clear_screen();
	output += oof::cursor_visibility(false);
	output += oof::reset_formatting();
	output += oof::bg_color({ 12, 12, 12 });
	output += oof::position(0, 0);
	FastWrite::Write(output);
}

void FastWrite::Write(std::string output) {
	HANDLE const output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	const auto char_count = static_cast<DWORD>(output.length());
	WriteConsoleA(output_handle, output.c_str(), char_count, nullptr, nullptr);
}
