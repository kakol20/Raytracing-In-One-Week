#include "String.h"

String::String(const char copyChar) {
	delete[] m_string;
	m_string = new char[2];
	m_string[0] = copyChar;
	m_string[1] = '\0';
}

String::String(const char* copyString) {
	delete[] m_string;

	const size_t size = strlen(copyString) + 1;
	m_string = new char[size];
	strcpy_s(m_string, size, copyString);
}

String::String(const std::string& copyString) : String(copyString.c_str()) {
}

String::String(const String& copyString) : String(copyString.GetChar()) {
}

String::~String() {
	delete[] m_string;
}

String& String::operator=(const char copyChar) {
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
	return this->operator+=(String(copyChar));
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

bool String::operator==(const char* otherString) const {
	return !strcmp(m_string, otherString);
}

bool String::operator==(const String& otherString) const {
	return this->operator==(otherString.GetChar());
}

bool String::operator!=(const char* otherString) const {
	return !(this->operator==(otherString));
}

bool String::operator!=(const String& otherString) const {
	return !(this->operator==(otherString));
}

bool String::operator<(const String& otherString) const {
	return strcmp(m_string, otherString.GetChar()) < 0;
}

const char* String::GetChar() const {
	return m_string;
}

size_t String::Length() {
	return strlen(m_string);
}

const char* String::GetFirst(const char* delimiter) const {
	const size_t SIZE = strlen(m_string) + 1;
	char* tempString = new char[SIZE];
	strcpy_s(tempString, SIZE, m_string);

	char* first;
	char* second = NULL;

	first = strtok_s(tempString, delimiter, &second);

	return first;
}

const char* String::GetSecond(const char* delimiter) const {
	const size_t SIZE = strlen(m_string) + 1;
	char* tempString = new char[SIZE];
	strcpy_s(tempString, SIZE, m_string);

	char* first;
	char* second = NULL;

	first = strtok_s(tempString, delimiter, &second);

	return second;
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

void String::Clear() {
	delete[] m_string;
	m_string = new char[1];
	m_string[0] = '\0';
}

std::istream& operator>>(std::istream& is, String& string) {
	string.Clear();

	char* temp = new char[512];
	is.getline(temp, 512);

	string = temp;
	delete[] temp;

	return is;
}

std::ostream& operator<<(std::ostream& os, const String& string) {
	os << string.GetChar();
	return os;
}