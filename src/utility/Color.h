#include <SFML/Main.hpp>

class Color {
public:
	struct FloatValue { float r; float g; float b; };
	struct RawValue { sf::Uint8 r; sf::Uint8 g; sf::Uint8 b; };

	Color();
	Color(const float& r, const float& g, const float& b);
	Color(const FloatValue& col);
	Color(const RawValue& col);
	Color(const sf::Uint8& r, const sf::Uint8& g, const sf::Uint8& b);
	~Color() {};

	// ----- ASSIGNMENT OPERATORS -----

	Color& operator=(const Color& other);
	Color& operator+=(const Color& other);
	Color& operator-=(const Color& other);
	Color& operator*=(const Color& other);
	Color& operator/=(const Color& other);

	// ----- ARITHMETIC OPERATORS -----

	inline Color operator+(const Color& other) const {
		Color out(m_val.r, m_val.g, m_val.b);
		out += other;
		return out;
	};
	inline Color operator-(const Color& other) const {
		Color out(m_val.r, m_val.g, m_val.b);
		out -= other;
		return out;
	};
	inline Color operator*(const Color& other) const {
		Color out(m_val.r, m_val.g, m_val.b);
		out *= other;
		return out;
	};
	inline Color operator/(const Color& other) const {
		Color out(m_val.r, m_val.g, m_val.b);
		out /= other;
		return out;
	};

	// ----- GETTERS & SETTERS -----

	Color::RawValue GetRawValue() const;
	inline Color::FloatValue GetFloatValue() const { return m_val; };


private:
	Color::FloatValue m_val;
};