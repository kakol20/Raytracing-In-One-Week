#include <Windows.h>

#define OOF_IMPL
#include "oof/oof.h"

#include "FastWrite.h"
#include "String.h"

auto enable_vt_mode() -> void;

int main() {
	enable_vt_mode();

	std::cout << oof::cursor_visibility(false);

	String output(oof::reset_formatting());
	output += oof::clear_screen();
	output += '\a';
	output += "Hello World!\n";
	output += String::ToString(8022022);
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