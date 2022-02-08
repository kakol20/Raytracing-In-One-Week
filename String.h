#pragma once

#include <iostream>
#include <string>

class String {
public:
	String(const char* string = "");
	String(const std::string& copyString);
	String(const String& copyString);
	~String();

	// ----- OPERATOR OVERLOADING -----

	String& operator=(const char* copyString);
	String& operator=(const std::string& copyString);
	String& operator=(const String& copyString);

	String& operator+=(const char* copyString);
	String& operator+=(const std::string& copyString);
	String& operator+=(const String& copyString);

	const char* GetChar() const;

	size_t Length();

private:
	char* m_string = new char[512];
};

