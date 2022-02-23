#include "String.h"

String::String(const char copyChar) {
	m_string = copyChar;
}

String::String(const char* copyString) {
	m_string = copyString;
}

String::String(const std::string& copyString) : String(copyString.c_str()) {
}

String::String(const String& copyString) : String(copyString.GetChar()) {
}

String::~String() {
}

String& String::operator=(const char copyChar) {
	m_string = copyChar;

	return *this;
}

String& String::operator=(const char* copyString) {
	m_string = copyString;

	return *this;
}

String& String::operator=(const std::string& copyString) {
	m_string = copyString;

	return *this;
}

String& String::operator=(const String& copyString) {
	if (this == &copyString) return *this;

	m_string = copyString.GetChar();

	return *this;
}

String& String::operator+=(const char copyChar) {
	m_string += copyChar;

	return *this;
}

String& String::operator+=(const char* copyString) {
	m_string += copyString;

	return *this;
}

String& String::operator+=(const std::string& copyString) {
	m_string += copyString;

	return *this;
}

String& String::operator+=(const String& copyString) {
	m_string += copyString.GetChar();

	return *this;
}

bool String::operator==(const char* otherString) const {
	return m_string == otherString;
}

bool String::operator==(const String& otherString) const {
	return m_string == otherString.GetChar();
}

bool String::operator!=(const char* otherString) const {
	return !(m_string == otherString);
}

bool String::operator!=(const String& otherString) const {
	return !(m_string == otherString);
}

bool String::operator<(const String& otherString) const {
	return m_string < otherString.GetChar();
}

const char* String::GetChar() const {
	return m_string.c_str();
}

size_t String::Length() {
	return m_string.length();
}

const std::string String::GetFirst(const char delimiter) const {
	std::string first;

	//bool out = true;
	for (auto c : m_string) {
		if (c == delimiter) {
			break;
		}
		else {
			first += c;
		}
	}

	return first;
}

const std::string String::GetSecond(const char delimiter) const {
	std::string second;

	bool firstOut = true;
	for (auto c : m_string) {
		if (firstOut) {
			if (c == delimiter) {
				firstOut = false;
			}
		}
		else {
			second += c;
		}
	}

	return second;
}

String String::ToString(const float number) {
	return String(std::to_string(number));
}

String String::ToString(const int number) {
	return String(std::to_string(number));
}

String String::ToString(const unsigned int number) {
	return String(std::to_string(number));
}

float String::ToFloat(const char* number) {
	//char* end;
	//float out = strtof(number, &end);
	////delete end;
	//end = nullptr;

	return std::stof(number);
}

float String::ToFloat(const std::string number) {
	return std::stof(number);
}

int String::ToInt(const char* number) {
	char* end;
	int out = (int)strtol(number, &end, 10);

	//delete end;
	end = nullptr;

	return out;
}

int String::ToInt(const std::string number) {
	return std::stoi(number);
}

unsigned int String::ToUInt(const std::string number) {
	return (unsigned int)std::stoul(number);
}

void String::Clear() {
	m_string.clear();
}

std::istream& operator>>(std::istream& is, String& string) {
	string.Clear();

	std::string tempString;

	std::getline(is, tempString);

	string = tempString;

	return is;
}

std::ostream& operator<<(std::ostream& os, const String& string) {
	os << string.GetChar();
	return os;
}