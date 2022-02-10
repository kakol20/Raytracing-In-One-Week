#include <Windows.h>

#include "FastWrite.h"

void FastWrite::Write(std::string& output) {
	HANDLE const output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	const auto char_count = static_cast<DWORD>(output.length());
	WriteConsoleA(output_handle, output.c_str(), char_count, nullptr, nullptr);

}

void FastWrite::Write(String& output) {
	HANDLE const output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	const auto char_count = static_cast<DWORD>(output.Length());
	WriteConsoleA(output_handle, output.GetChar(), char_count, nullptr, nullptr);
}
