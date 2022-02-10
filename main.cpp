#include <Windows.h>

#define OOF_IMPL
#include "oof/oof.h"

#include "FastWrite.h"
#include "Random.h"
#include "String.h"

auto enable_vt_mode() -> void;

thread_local unsigned int Random::Seed = 10022022;

int main() {
	enable_vt_mode();

	std::cout << oof::cursor_visibility(false);

	String output("");
	output = oof::reset_formatting();
	output += oof::clear_screen();
	output += oof::bg_color({ 12, 12, 12 });
	output += '\a';
	output += "Hello World!\n";
	output += String::ToString(8022022);
	output += '\n';
	output += "Random unsigned int = ";
	output += String::ToString(Random::RandomUInt());
	output += '\n';
	output += "Random int = ";
	output += String::ToString(Random::RandInt());
	output += '\n';
	output += "Max Int = ";
	output += String::ToString(Random::MaxUInt);
	output += '\n';
	output += "Random float = ";
	output += String::ToString(Random::RandFloat());
	output += '\n';
	output += "Random float between -1 and 1 = ";
	output += String::ToString(Random::RandFloatRange(-1, 1));
	output += '\n';
	output += "Seed After = ";
	output += String::ToString(Random::Seed);
	output += '\n';

	FastWrite::Write(output);

	system("pause");

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