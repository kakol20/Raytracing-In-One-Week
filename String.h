#pragma once

#include <iostream>
#include <string>

class String {
public:
	String(const char copyChar);
	String(const char* copyString = "");
	String(const std::string& copyString);
	String(const String& copyString);
	~String();

	// ----- ASSIGNMENT -----

	String& operator=(const char copyChar);
	String& operator=(const char* copyString);
	String& operator=(const std::string& copyString);
	String& operator=(const String& copyString);

	String& operator+=(const char copyChar);
	String& operator+=(const char* copyString);
	String& operator+=(const std::string& copyString);
	String& operator+=(const String& copyString);

	// ----- COMPARISON -----

	bool operator==(const char* otherString) const;
	bool operator==(const String& otherString) const;

	bool operator!=(const char* otherString) const;
	bool operator!=(const String& otherString) const;

	bool operator<(const String& otherString) const;

	// ----- GETTERS -----

	const char* GetChar() const;
	size_t Length();

	const std::string GetFirst(const char delimiter) const;
	const std::string GetSecond(const char delimiter) const;

	// ----- IOSTREAM -----
	friend std::istream& operator>>(std::istream& is, String& string);
	friend std::ostream& operator<<(std::ostream& os, const String& string);

	// ----- CONVERSIONS -----

	static String ToString(const float number);
	static String ToString(const int number);
	static String ToString(const unsigned int number);

	static float ToFloat(const char* number);
	static float ToFloat(const std::string number);
	static int ToInt(const char* number);
	static int ToInt(const std::string number);
	static unsigned int ToUInt(const std::string number);

	// ----- OTHER -----
	void Clear();
private:
	//char* m_string = new char[256];
	std::string m_string;
};
