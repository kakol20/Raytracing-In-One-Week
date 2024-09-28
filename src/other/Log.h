#pragma once

#include <string>
#include <chrono>

class Log {
public:
	Log() {};
	~Log() {};

	static void Write(const std::string input);
	static void WriteOneLine(const std::string input);

	static void EndLine();
	static void StartLine();

	static void Save(const bool overwrite = true);

	static void StartTime();
	static bool CheckTime(const long long milliseconds);
	static bool CheckTimeSeconds(const double seconds);

	static std::string ToString(const double value, const unsigned int precision = 6);
	static std::string ToString(const size_t value, const unsigned int precision = 0, const char lead = '0');
	static std::string ToString(const unsigned int value, const unsigned int precision = 0, const char lead = '0');
	
	static std::string LeadingCharacter(const std::string value, const unsigned int amount = 0, const char lead = ' ');

private:
	static std::string m_console;
	static std::chrono::steady_clock::time_point m_time;
};