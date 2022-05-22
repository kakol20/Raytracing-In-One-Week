#ifndef HDR_H
#define HDR_H

#include "Float.h"

class HDR {
public:
	enum class Interpolation { Linear, Closest, Cubic };
	enum class Extrapolation { Repeat, Extend, Clip };
	enum class ColorSpace { Non_Color, sRGB };

public:
	HDR(const char* file = "", const Interpolation interpolation = Interpolation::Closest, const Extrapolation extrapolation = Extrapolation::Repeat, const ColorSpace colorSpace = ColorSpace::Non_Color);
	HDR(const HDR& copyHDR);
	~HDR();

	HDR& operator=(const HDR& copyHDR);

	/// <summary>
	/// Low level access to data
	/// </summary>
	/// <param name="i"></param>
	/// <returns></returns>
	inline float& operator[](size_t i) { return m_data[i]; };

	void GetColor(const Float& x, const Float& y, Float& r, Float& g, Float& b);

	inline int GetHeight() const { return m_h; };
	inline int GetWidth() const { return m_w; };
	inline size_t GetSize() const { return m_size; };

private:
	float* m_data;
	int m_channels;
	int m_h;
	int m_w;
	size_t m_size;

	Interpolation m_interpolation;
	Extrapolation m_extrapolation;
	ColorSpace m_colorSpace;

	bool Read(const char* file);

	void SetColor(const int& x, const int& y, const Float& r, const Float& g, const Float& b);

	int GetIndex(const int& x, const int& y);
	void MissingTexture();

	// ----- INTERPOLATION FUNCTIONS -----

	void Bicubic(const Float& x, const Float& y, Float& r, Float& g, Float& b);
	void Bilinear(const Float& x, const Float& y, Float& r, Float& g, Float& b);
	void NearestNeighbour(const Float& x, const Float& y, Float& r, Float& g, Float& b);
};
#endif // !HDR_H
