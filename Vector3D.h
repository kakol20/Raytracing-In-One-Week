#pragma once
class Vector3D {
public:
	Vector3D();
	Vector3D(const float scalar);
	Vector3D(const float x, const float y, const float z = 0.f);
	Vector3D(const Vector3D& copyVector);
	~Vector3D();

private:
	float m_x, m_y, m_z;

public:
	// ----- GETTERS -----

	float GetX() { return m_x; };
	float GetY() { return m_y; };
	float GetZ() { return m_z; };

	// ----- LENGTH -----

	float Magnitude();
	float SqrMagnitude();
	void Normalize();

	// ----- ALGERBAIC OPERATIONS -----

	static float DotProduct(const Vector3D& v1, const Vector3D& v2);
	static Vector3D CrossProduct(const Vector3D& v1, const Vector3D& v2);
	static Vector3D Rotate(const Vector3D& v, const Vector3D& radians);

	// ----- OTHER -----

	bool NearZero();
	bool Threshold(const float threshold);
	void Abs();
	bool IsNAN();

	static Vector3D Clamp(const Vector3D& v, const float min, const float max);
	static Vector3D Lerp(const Vector3D& from, const Vector3D& to, const float factor);
	//static Vector3D Slerp(const Vector3D& from, const Vector3D& to, const float t);

	void UVSphere(float& u, float& v);

	// ----- COLOR FUNCTIONS -----

	/// <summary>
	/// Convert HSV to RGB
	/// </summary>
	/// <param name="h">0&lt;h&lt;360</param>
	/// <param name="s">0&lt;s&lt;1</param>
	/// <param name="v">0&lt;v&lt;1</param>
	/// <returns></returns>
	static Vector3D HSVtoRGB(const float h, const float s, const float v);

	static Vector3D KelvinToRGB(const float kelvin);
	static Vector3D OrderedDithering(const Vector3D col, const int x, const int y, const int factor);

public: // ----- STATIC RANDOM FUNCTIONS -----

	static Vector3D Random(const float min, const float max);
	static Vector3D RandomInHemisphere(const Vector3D& normal);
	static Vector3D RandomInUnitDisk();
	static Vector3D RandomInUnitSphere();
	static Vector3D RandomUnitVector();

public: // ----- OPERATOR OVERLOADING -----
	// https://en.cppreference.com/w/cpp/language/operators

	Vector3D& operator=(const float& scalar);
	Vector3D& operator=(const Vector3D& copyVector);

	// ----- DIVISION -----

	Vector3D operator/(const float scalar) const;
	Vector3D operator/(const Vector3D& otherVector) const;
	Vector3D& operator/=(const float scalar);
	Vector3D& operator/=(const Vector3D& otherVector);

	// ----- MULTIPLICATION -----

	Vector3D operator*(const float scalar) const;
	Vector3D operator*(const Vector3D& otherVector) const;
	Vector3D& operator*=(const float scalar);
	Vector3D& operator*=(const Vector3D& otherVector);

	// ----- ADDITION -----

	Vector3D operator+(const Vector3D& otherVector) const;
	Vector3D operator++(int i);
	Vector3D& operator++();
	Vector3D& operator+=(const Vector3D& otherVector);

	// ----- SUBTRACTION -----

	Vector3D operator-(const Vector3D& otherVector) const;
	Vector3D operator--(int i);
	Vector3D& operator--();
	Vector3D& operator-=(const Vector3D& otherVector);

	// ----- COMPARISON -----

	bool operator==(const Vector3D& otherVector) const;
	bool operator!=(const Vector3D& otherVector) const;

	// ----- OTHER OPERATORS -----

	Vector3D operator-() const;
};
