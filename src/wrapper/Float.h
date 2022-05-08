#ifndef FLOAT_H
#define FLOAT_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>

/// <summary>
	/// A class that wraps functionality of floating point numbers including mathematical functions.
	/// Also includes conversions to other types
	/// </summary>
class Float {
public:
#ifdef WIN32
	typedef float ForD;
#else
	typedef double ForD;
#endif // WIN32

	inline Float(const double& num) { m_num = (ForD)num; };
	inline Float(const float& num) { m_num = (ForD)num; };
	inline Float(const Float& num) { m_num = num.m_num; };
	inline Float(const int& num = 0) { m_num = (ForD)num; };
	inline Float(const unsigned int& num) { m_num = (ForD)num; };
	~Float() {};

	// ----- ASSIGNMENT -----

	inline Float& operator=(const double& num) {
		m_num = (Float::ForD)num;
		return *this;
	};
	inline Float& operator=(const float& num) {
		m_num = (Float::ForD)num;
		return *this;
	};
	inline Float& operator=(const Float& num) {
		if (this == &num) return *this;
		m_num = num.m_num;
		return *this;
	};
	inline Float& operator=(const int& num) {
		m_num = (Float::ForD)num;
		return *this;
	};
	inline Float& operator=(const unsigned int& num) {
		m_num = (Float::ForD)num;
		return *this;
	};

	inline Float& operator%=(const Float& n) {
		m_num = std::fmod(m_num, n.m_num);
		return *this;
	};
	inline Float& operator*=(const Float& n) {
		m_num *= n.m_num;
		return *this;
	};
	inline Float& operator/=(const Float& n) {
		m_num /= n.m_num;
		return *this;
	};
	inline Float& operator+=(const Float& n) {
		m_num += n.m_num;
		return *this;
	};
	inline Float& operator-=(const Float& n) {
		m_num -= n.m_num;
		return *this;
	};

	// ----- ARITHMETIC -----

	inline Float operator%(const Float& n) const {
		return Float(std::fmod(m_num, n.m_num));
	};
	inline Float operator-(const Float& n) const {
		return Float(m_num - n.m_num);
	};
	inline Float operator*(const Float& n) const {
		return Float(m_num * n.m_num);
	};
	inline Float operator/(const Float& n) const {
		return Float(m_num / n.m_num);
	};
	inline Float operator+(const Float& n) const {
		return Float(m_num + n.m_num);
	};

	inline friend Float operator-(const double& left, const Float& right) {
		return Float((Float::ForD)left - right.m_num);
	};
	inline friend Float operator-(const float& left, const Float& right) {
		return Float((Float::ForD)left - right.m_num);
	};
	inline friend Float operator-(const int& left, const Float& right) {
		return Float((Float::ForD)left - right.m_num);
	};
	inline friend Float operator*(const double& left, const Float& right) {
		return Float((Float::ForD)left * right.m_num);
	};
	inline friend Float operator*(const float& left, const Float& right) {
		return Float((Float::ForD)left * right.m_num);
	};
	inline friend Float operator*(const int& left, const Float& right) {
		return Float((Float::ForD)left * right.m_num);
	};
	inline friend Float operator/(const double& left, const Float& right) {
		return Float((Float::ForD)left / right.m_num);
	};
	inline friend Float operator/(const float& left, const Float& right) {
		return Float((Float::ForD)left / right.m_num);
	};
	inline friend Float operator/(const int& left, const Float& right) {
		return Float((Float::ForD)left / right.m_num);
	};
	inline friend Float operator+(const double& left, const Float& right) {
		return Float((Float::ForD)left + right.m_num);
	};
	inline friend Float operator+(const float& left, const Float& right) {
		return Float((Float::ForD)left + right.m_num);
	};
	inline friend Float operator+(const int& left, const Float& right) {
		return Float((Float::ForD)left + right.m_num);
	};

	/// <summary>
	/// Shorthand for multiplying by -1;
	/// </summary>
	/// <returns></returns>
	inline Float operator-() const {
		return Float(m_num * -1);
	};

	// ----- COMPARISON OPERATORS -----

	inline bool operator!=(const Float& n) const {
		return m_num != n.m_num;
	};
	inline bool operator<(const Float& n) const {
		return m_num < n.m_num;
	};
	inline bool operator<=(const Float& n) const {
		return m_num <= n.m_num;
	};
	inline bool operator==(const Float& n) const {
		return m_num == n.m_num;
	};
	inline bool operator>(const Float& n) const {
		return m_num > n.m_num;
	};
	inline bool operator>=(const Float& n) const {
		return m_num >= n.m_num;
	};

	// ----- IOSTREAM -----

	inline friend std::istream& operator>>(std::istream& is, Float& num) { is >> num.m_num; return is; };
	inline friend std::ostream& operator<<(std::ostream& os, const Float& num) { os << num.m_num; return os; };

	// ----- OTHER -----

	// -- Static Variables --

	static const Float MaxVal;
	static const Float MinVal;
	static const Float PI;
	static const Float TAU;
	static const Float ToRadians;
	static const ForD NearZero;

	// -- Conversion --

	inline Float::ForD GetValue() const {
		return m_num;
	};
	inline int ToInt() const {
		return (int)m_num;
	};
	inline unsigned int ToUInt() const {
		return (unsigned int)m_num;
	};

	static Float FromString(const std::string& value);
	static std::string ToString(const Float& num);

	// -- Trigonometry --

	static Float Asin(const Float& x);
	static Float Atan2(const Float& y, const Float& x);
	static Float Cos(const Float& x);
	static Float Sin(const Float& x);
	static Float Tan(const Float& v);

	// -- Modify --

	static Float Abs(const Float& v);
	static Float Ceil(const Float& v);
	static Float Clamp(const Float& v, const Float& lo, const Float& hi);
	static Float Floor(const Float& v);
	static Float Pow(const Float& base, const Float& exponent);
	static Float Round(const Float& v);
	static Float Sqrt(const Float& v);

	static Float Max(const Float& a, const Float& b);
	static Float Min(const Float& a, const Float& b);

	// -- Interpolation --

	static Float Lerp(const Float& from, const Float& to, const Float& factor, bool clamp = false);
	static Float Map(const Float& v, const Float& fromMin, const Float& fromMax, const Float& toMin, const Float& toMax, const bool& clamp = false);

	// https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references

	/// <summary>
	/// Cubic interpolation
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b">lower value</param>
	/// <param name="c">upper value</param>
	/// <param name="d"></param>
	/// <param name="factor">value between 0 &amp; 1</param>
	/// <returns></returns>
	static Float Cubic(const Float& a, const Float& b, const Float& c, const Float& d, const Float& factor);

private:
	ForD m_num;
};

#endif // !FLOAT_H
