#ifndef IMAGE_H
#define IMAGE_H

#include "Float.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

class Image {
public:
	enum class Interpolation { Linear, Closest, Cubic };
	enum class Extrapolation { Repeat, Extend, Clip };
	enum class ColorSpace { Non_Color, sRGB };

	enum class FileType { PNG, JPG, BMP, TGA, NONE };

public:
	Image(const char* file, const Interpolation interpolation, const Extrapolation extrapolation, const ColorSpace colorSpace);
	Image(const Image& copyImage);
	Image(const int w = 0, const int h = 0, const int channels = 0, const Interpolation interpolation = Interpolation::Closest, const Extrapolation extrapolation = Extrapolation::Repeat, const ColorSpace colorSpace = ColorSpace::Non_Color);

	~Image();

	Image& operator=(const Image& copyImage);

	bool Write(const char* file);

	void GetColor(const Float x, const Float y, Float& r, Float& g, Float& b);
	void SetColor(const int x, const int y, const Float r, const Float g, const Float b);

	int GetHeight() const { return m_h; };
	int GetWidth() const { return m_w; };

	/*size_t GetSize() const { return m_size; };

	inline float& operator[](size_t i) { return m_dataF[i].GetValue(); };*/

	void SetColorSpace(const ColorSpace colorSpace) { m_colorSpace = colorSpace; };

	void FloydSteinbergDither(const int factor = 255);
	void OrderedDither(const int factor = 255);

	static Float LinearToSRGB(const Float color);
	static Float sRGBToLinear(const Float color);

private:
	size_t m_size;
	int m_w;
	int m_h;
	int m_channels;

	Interpolation m_interpolation;
	Extrapolation m_extrapolation;
	ColorSpace m_colorSpace;

	// ----- DATA -----

	uint8_t* m_data;
	Float* m_dataF;

	// ----- FUNCTIONS -----

	bool Read(const char* file);

	FileType GetFileType(const char* file);
	int GetIndex(const int x, const int y);
	void MissingTexture();

	void FromColorSpace();
	void IndexToXY(const size_t index, int& x, int& y);
	void ToColorSpace();

	// ----- INTERPOLATION FUNCTIONS -----

	void Bicubic(const Float x, const Float y, Float& r, Float& g, Float& b);
	void Bilinear(const Float x, const Float y, Float& r, Float& g, Float& b);
	void NearestNeighbour(const Float x, const Float y, Float& r, Float& g, Float& b);

	// ----- PRIVATE STATIC -----

	// private variable
	static const unsigned int Threshold[];
};

#endif // !IMAGE_H
