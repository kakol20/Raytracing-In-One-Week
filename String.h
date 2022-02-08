#pragma once

#include <iostream>
#include <string>

class String {
public:
	String(const char copyChar);
	String(const char* string = "");
	String(const std::string& copyString);
	String(const String& copyString);
	~String();

	// ----- OPERATOR OVERLOADING -----

	String& operator=(const char copyChar);
	String& operator=(const char* copyString);
	String& operator=(const std::string& copyString);
	String& operator=(const String& copyString);

	String& operator+=(const char copyChar);
	String& operator+=(const char* copyString);
	String& operator+=(const std::string& copyString);
	String& operator+=(const String& copyString);

	// ----- GETTERS -----

	const char* GetChar() const;
	size_t Length();

	// ----- CONVERSIONS -----

	static String ToString(const float number);
	static String ToString(const int number);

	static float ToFloat(const char* number);
	static int ToInt(const char* number);
private:
	char* m_string = new char[256];
};
