#include "String.h"

String::String(const char copyChar) {
	delete[] m_string;
	m_string = new char[2];
	m_string[0] = copyChar;
	m_string[1] = '\0';
}

String::String(const char* string) {
	delete[] m_string;

	const size_t size = strlen(string) + 1;
	m_string = new char[size];
	strcpy_s(m_string, size, string);
}

String::String(const std::string& copyString) : String(copyString.c_str()) {
}

String::String(const String& copyString) : String(copyString.GetChar()) {
}

String::~String() {
	delete[] m_string;
}

String& String::operator=(const char copyChar) {
	// TODO: insert return statement here
	delete[] m_string;
	m_string = new char[2];
	m_string[0] = copyChar;
	m_string[1] = '\0';

	return *this;
}

String& String::operator=(const char* copyString) {
	delete[] m_string;

	const size_t size = strlen(copyString) + 1;
	m_string = new char[size];
	strcpy_s(m_string, size, copyString);

	return *this;
}

String& String::operator=(const std::string& copyString) {
	return this->operator=(copyString.c_str());
}

String& String::operator=(const String& copyString) {
	if (this == &copyString) return *this;

	return this->operator=(copyString.GetChar());
}

String& String::operator+=(const char copyChar) {
	// TODO: insert return statement here
	String temp(copyChar);

	return this->operator+=(temp);
}

String& String::operator+=(const char* copyString) {
	size_t size = strlen(m_string) + strlen(copyString) + 1;
	char* temp = new char[size];

	strcpy_s(temp, size, m_string);
	strcat_s(temp, size, copyString);

	delete[] m_string;
	m_string = new char[size];
	strcpy_s(m_string, size, temp);

	return *this;
}

String& String::operator+=(const std::string& copyString) {
	return this->operator+=(copyString.c_str());
}

String& String::operator+=(const String& copyString) {
	return this->operator+=(copyString.GetChar());
}

const char* String::GetChar() const {
	return m_string;
}

size_t String::Length() {
	return strlen(m_string);
}

String String::ToString(const float number) {
	return String(std::to_string(number));
}

String String::ToString(const int number) {
	return String(std::to_string(number));
}

float String::ToFloat(const char* number) {
	char* end;
	return strtof(number, &end);
}

int String::ToInt(const char* number) {
	char* end;
	return (int)strtol(number, &end, 10);
}
