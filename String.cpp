#include "String.h"

bool operator==(const String& first, const String& second) {
	//!strcmp(m_string, copyString.m_string);
	return !strcmp(first.m_string, second.m_string);
}

bool operator<(const String& first, const String& second) {
	return strcmp(first.m_string, second.m_string) < 0;
}

std::ostream& operator<<(std::ostream& os, const String& string)
{
	os << string.GetChar();

	return os;
}

std::istream& operator>>(std::istream& is, String& string)
{
	string.Clear();

	char* temp = new char[280];

	is.getline(temp, 280);

	string = temp;

	delete[] temp;

	return is;
}

// ------------------------------------------------------------

String::String(const char* string)
{
	const size_t SIZE = strlen(string) + 1;

	m_string = new char[SIZE];

	strcpy_s(m_string, SIZE, string);
}

String::String(const String& copyString)
{
	//m_CopyString(copyString.m_string);
	const size_t SIZE = strlen(copyString.m_string) + 1;

	m_string = new char[SIZE];

	strcpy_s(m_string, SIZE, copyString.m_string);
}

String& String::operator=(const String& copyString)
{
	if (&copyString == this) return *this;

	return this->operator=(copyString.m_string);
}

String& String::operator=(const char* string)
{
	delete[] m_string;
	//m_string = nullptr;

	const size_t SIZE = strlen(string) + 1;

	m_string = new char[SIZE];

	strcpy_s(m_string, SIZE, string);

	return *this;
}

/// <summary>
/// Get string as char*
/// </summary>
/// <returns></returns>
const char* String::GetChar() const
{
	return m_string;
}

/// <summary>
/// Get string before delimiter
/// </summary>
/// <param name="delimiter"></param>
/// <returns></returns>
const char* String::GetFirst(const char* delimiter) const {
	const size_t SIZE = strlen(m_string) + 1;
	char* tempString = new char[SIZE];
	strcpy_s(tempString, SIZE, m_string);

	char* first;
	char* second = NULL;

	first = strtok_s(tempString, delimiter, &second);

	return first;
}

/// <summary>
/// Get string after a delimiter
/// </summary>
/// <param name="delimiter"></param>
/// <returns></returns>
const char* String::GetSecond(const char* delimiter) const
{
	const size_t SIZE = strlen(m_string) + 1;
	char* tempString = new char[SIZE];
	strcpy_s(tempString, SIZE, m_string);

	char* first;
	char* second = NULL;

	first = strtok_s(tempString, delimiter, &second);

	//return strrchr(m_string, delimiter);
	return second;
}

String String::operator+(const String& copyString)
{
	return this->operator+(copyString.m_string);
}

String String::operator+(const char* string)
{
	/*size_t SIZE = strlen(m_string) + strlen(string) + 1;

	char* temp = new char[SIZE];

	strcpy_s(temp, SIZE, m_string);
	strcat_s(temp, SIZE, string);*/

	String newString = m_string;
	newString += string;

	return newString;
}

String& String::operator+=(const String& copyString)
{
	return this->operator+=(copyString.m_string);
}

String& String::operator+=(const char* string)
{
	size_t SIZE = strlen(m_string) + strlen(string) + 1;

	char* temp = new char[SIZE];

	strcpy_s(temp, SIZE, m_string);
	strcat_s(temp, SIZE, string);

	delete[] m_string;
	//m_string = nullptr;

	m_string = new char[SIZE];

	strcpy_s(m_string, SIZE, temp);

	return *this;
}

bool String::operator==(const String& copyString)
{
	return !strcmp(m_string, copyString.m_string);
}

bool String::operator==(const char* string)
{
	return !strcmp(m_string, string);
}

bool String::operator!=(const String& copyString)
{
	return !(this->operator==(copyString));
}

bool String::operator!=(const char* string)
{
	return !(this->operator==(string));
}

//bool String::operator<(const String& copyString) {
//	return strcmp(m_string, copyString.m_string) < 0;
//}
//
//bool String::operator<(const char* string) {
//	return strcmp(m_string, string) < 0;
//}

size_t String::Length() const
{
	return strlen(m_string);
}

char String::operator[](int index) const
{
	return m_string[index];
}

void String::Clear()
{
	*this = "";
}

bool String::IsEmpty()
{
	return !strcmp(m_string, "");
}

int String::Find(char string)
{
	for (size_t i = 0; i < strlen(m_string); i++)
	{
		if (m_string[i] == string) return (int)i;
	}

	return -1;
}

/// <summary>
/// Find a character in string and return its index
/// </summary>
/// <param name="string"></param>
/// <returns></returns>
int String::Find(const char* string)
{
	for (size_t i = 0; i < strlen(m_string); i++)
	{
		if (m_string[i] == string[0])
		{
			bool continuing = true;

			if (i + strlen(string) <= strlen(m_string))
			{
				for (size_t j = 1; j < strlen(string); j++)
				{
					if (m_string[i + j] != string[j]) continuing = false;
				}
			}
			else
			{
				return -1;
			}

			if (continuing) return (int)i;
		}
	}

	return -1;
}

/// <summary>
/// Count how many times a character appears in string
/// </summary>
/// <param name="string"></param>
/// <returns></returns>
int String::Count(char string)
{
	int count = 0;

	for (size_t i = 0; i < strlen(m_string); i++)
	{
		if (m_string[i] == string) count++;
	}

	return count;
}

int String::ToInt(const String string) {
	const char* stringChar = string.GetChar();
	char* end;

	int out = strtol(stringChar, &end, 10);
	return out;
}

int String::ToInt(const char* string) {
	char* end;
	int out = strtol(string, &end, 10);
	return out;
}

float String::ToFloat(const char* string) {
	char* end;
	float out = strtof(string, &end);
	return out;
}

String::~String()
{
	delete[] m_string;
	m_string = nullptr;
}