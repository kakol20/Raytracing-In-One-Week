#ifndef STRING_H
#define STRING_H

#include <iostream>

class String
{
public:

	//String();
	String(const char* string = "");
	String(const String& copyString);

	String& operator=(const String& copyString);
	String& operator=(const char* string);

	const char* GetChar() const;

	const char* GetFirst(const char* delimiter) const;
	const char* GetSecond(const char* delimiter) const;

	String operator+(const String& copyString);
	String operator+(const char* string);

	String& operator+=(const String& copyString);
	String& operator+=(const char* string);

	bool operator==(const String& copyString);
	bool operator==(const char* string);

	bool operator!=(const String& copyString);
	bool operator!=(const char* string);

	friend bool operator==(const String& first, const String& second);
	friend bool operator<(const String& first, const String& second);

	size_t Length() const;

	char operator[](int index) const;

	void Clear();

	bool IsEmpty();

	int Find(char string);
	int Find(const char* string);

	int Count(char string);

	static int ToInt(const String string);
	static int ToInt(const char* string);

	static float ToFloat(const char* string);

	~String();

private:

	char* m_string = new char[256];
};

std::ostream& operator<<(std::ostream& os, const String& string);
std::istream& operator>>(std::istream& is, String& string);

#endif // !STRING_H
