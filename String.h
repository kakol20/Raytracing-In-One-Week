#pragma once

#include <iostream>

class String {
public:
	String(const char* string = "");
	~String();

	const char* GetChar() const;

	size_t Length();

private:
	char* m_string = new char[512];
};

