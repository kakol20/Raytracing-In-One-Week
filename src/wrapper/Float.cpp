#include <sstream>
#include <iomanip>

#include "Float.h"

Float Float::FromString(const std::string& value) {
	std::stringstream ss(value);
	Float out = 0;
	ss >> out;
	return out;
}

std::string Float::ToString(const Float& num) {
	std::stringstream ss("");
	ss << std::setprecision(6);
	ss << num.m_num;

	std::string out = "";
	ss >> out;
	return out;
}

Float Float::Asin(const Float& x) {
	return Float(std::asin(x.m_num));
}

Float Float::Atan2(const Float& y, const Float& x) {
	return Float(std::atan2(y.m_num, x.m_num));
}

Float Float::Cos(const Float& x) {
	return Float(std::cos(x.m_num));
}

Float Float::Sin(const Float& x) {
	return Float(std::sin(x.m_num));
}

Float Float::Tan(const Float& v) {
	return Float(std::tan(v.m_num));
}

Float Float::Abs(const Float& v) {
	return Float(std::abs(v.m_num));
}

Float Float::Ceil(const Float& v) {
	return Float(std::ceil(v.m_num));
}

Float Float::Clamp(const Float& v, const Float& lo, const Float& hi) {
	return Float(std::clamp(v, lo, hi));
}

Float Float::Floor(const Float& v) {
	return Float(std::floor(v.m_num));
}

Float Float::Pow(const Float& base, const Float& exponent) {
	return Float(std::pow(base.m_num, exponent.m_num));
}

Float Float::Round(const Float& v) {
	return Float(std::round(v.m_num));
}

Float Float::Sqrt(const Float& v) {
	return v == 1 ? Float(1) : Float(std::sqrt(v.m_num));
}

Float Float::Max(const Float& a, const Float& b) {
	return a >= b ? a : b;
}

Float Float::Min(const Float& a, const Float& b) {
	return a <= b ? a : b;
}

Float Float::Lerp(const Float& from, const Float& to, const Float& factor, bool clamp) {
	Float out = ((to - from) * factor) + from;

	if (clamp) out = Float::Clamp(out, from, to);
	return out;
}

Float Float::Map(const Float& v, const Float& fromMin, const Float& fromMax, const Float& toMin, const Float& toMax, const bool& clamp) {
	Float factor = (v - fromMin) / (fromMax - fromMin);
	return Float::Lerp(toMin, toMax, factor, clamp);
}

Float Float::Cubic(const Float& a, const Float& b, const Float& c, const Float& d, const Float& factor) {
	return b + 0.5 * factor * (c - a + factor * (2 * a - 5 * b + 4 * c - d + factor * (3 * (b - c) + d - a)));
}