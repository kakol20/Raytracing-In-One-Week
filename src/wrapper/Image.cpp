#include <algorithm>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "../../ext/stb/stb_image.h"
#include "../../ext/stb/stb_image_write.h"

#include "Image.h"

Image::Image(const Image& copyImage) {
	stbi_image_free(m_data);
	delete[] m_dataF;
	m_dataF = nullptr;

	m_w = copyImage.m_w;
	m_h = copyImage.m_h;
	m_channels = copyImage.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new uint8_t[m_size];
	m_dataF = new Float[m_size];

	memcpy(m_data, copyImage.m_data, m_size);

	for (size_t i = 0; i < m_size; i++) {
		m_dataF[i] = (int)m_data[i];
	}

	m_interpolation = copyImage.m_interpolation;
	m_extrapolation = copyImage.m_extrapolation;
	m_colorSpace = copyImage.m_colorSpace;
}

Image::Image(const int w, const int h, const int channels, const Interpolation interpolation, const Extrapolation extrapolation, const ColorSpace colorSpace) {
	if (w == 0 || h == 0 || channels == 0) {
		MissingTexture();
	}
	else {
		m_w = w;
		m_h = h;
		m_channels = channels;

		m_interpolation = interpolation;
		m_extrapolation = extrapolation;
		m_colorSpace = colorSpace;

		m_size = (size_t)(m_w * m_h * m_channels);

		m_data = new uint8_t[m_size];
		m_dataF = new Float[m_size];

		for (size_t i = 0; i < m_size; i++) {
			m_data[i] = 0;
			m_dataF[i] = 0;
		}
	}
}

Image::Image(const char* file, const Interpolation interpolation, const Extrapolation extrapolation, const ColorSpace colorSpace) {
	m_interpolation = interpolation;
	m_extrapolation = extrapolation;
	m_colorSpace = colorSpace;

	if (!Read(file)) {
		MissingTexture();
	}
}

Image::~Image() {
	stbi_image_free(m_data);
	delete[] m_dataF;
	m_dataF = nullptr;
}

Image& Image::operator=(const Image& copyImage) {
	if (&copyImage == this) return *this;

	stbi_image_free(m_data);
	delete[] m_dataF;
	m_dataF = nullptr;

	m_w = copyImage.m_w;
	m_h = copyImage.m_h;
	m_channels = copyImage.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new uint8_t[m_size];
	m_dataF = new Float[m_size];

	memcpy(m_data, copyImage.m_data, m_size);

	m_interpolation = copyImage.m_interpolation;
	m_extrapolation = copyImage.m_extrapolation;
	m_colorSpace = copyImage.m_colorSpace;

	for (size_t i = 0; i < m_size; i++) {
		m_dataF[i] = (int)m_data[i];
	}

	return *this;
}

bool Image::Read(const char* file) {
	m_data = stbi_load(file, &m_w, &m_h, &m_channels, 0);

	if (m_data == NULL) return false;

	m_size = (size_t)m_w * m_h * m_channels;

	m_dataF = new Float[m_size];
	for (size_t i = 0; i < m_size; i++) {
		//m_dataF[i] = 0;
		//m_dataF[i] = (int)m_data[i];
		Float val = (int)m_data[i];

		if (m_colorSpace == ColorSpace::sRGB) {
			val = Image::sRGBToLinear(val);
		}

		m_dataF[i] = val;
		m_data[i] = (uint8_t)Float::Round(Float::Clamp(val, 0, 255)).ToUInt();
	}

#ifdef _DEBUG
	std::cout << "Read image successful: " << file << '\n';
#endif // _DEBUG

	return true;
}

bool Image::Write(const char* file) {
	//ToColorSpace();

	Image::FileType type = GetFileType(file);

	int success = 0;

	for (size_t i = 0; i < m_size; i++) {
		Float val = m_dataF[i];

		if (m_colorSpace == ColorSpace::sRGB) {
			val = Image::LinearToSRGB(val);
		}

		m_data[i] = (uint8_t)Float::Clamp(val, 0, 255).ToUInt();

		//m_data[i] = Float::Round(Float::Clamp(m_dataF[i], 0, 255)).ToUInt();
	}

	//Dither(255);

	switch (type) {
	case Image::FileType::BMP:
		success = stbi_write_bmp(file, m_w, m_h, m_channels, m_data);
		break;

	case Image::FileType::JPG:
		success = stbi_write_jpg(file, m_w, m_h, m_channels, m_data, 100);
		break;

	case Image::FileType::PNG:
		success = stbi_write_png(file, m_w, m_h, m_channels, m_data, m_w * m_channels);
		break;

	case Image::FileType::TGA:
		success = stbi_write_tga(file, m_w, m_h, m_channels, m_data);
		break;
	case Image::FileType::NONE:
		success = 0;
		break;
	default:
		success = 0;
		break;
	}

#ifdef _DEBUG
	if (success == 0) {
		std::cout << "Error writing file: " << file << '\n';
		//std::cin.ignore();
	}
	else {
		std::cout << "Write image successful: " << file << '\n';
	}
#endif

	//FromColorSpace();

	return success != 0;
}

void Image::GetColor(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	if (m_extrapolation == Extrapolation::Clip && (x < 0 || y < 0 || x >= m_w || y >= m_h)) {
		r = 0;
		g = 0;
		b = 0;
	}
	else {
		Float l_x = m_extrapolation == Extrapolation::Clip ? Float::Clamp(x, 0, m_w) : x;
		Float l_y = m_extrapolation == Extrapolation::Clip ? Float::Clamp(y, 0, m_h) : y;

		if (m_interpolation == Interpolation::Linear) {
			// bilinear interpolation
			Bilinear(l_x, l_y, r, g, b);
		}
		else if (m_interpolation == Interpolation::Cubic) {
			// bicubic interpolation
			Bicubic(l_x, l_y, r, g, b);
		}
		else {
			// nearest neighbour
			NearestNeighbour(l_x, l_y, r, g, b);
		}
	}
}

void Image::SetColor(const int& x, const int& y, const Float& r, const Float& g, const Float& b) {
	int index = GetIndex(x, y);

	if (m_channels >= 3) {
		Float l_r = Float::Clamp(r, 0, 255);
		Float l_g = Float::Clamp(g, 0, 255);
		Float l_b = Float::Clamp(b, 0, 255);

		m_dataF[index + 0] = l_r;
		m_dataF[index + 1] = l_g;
		m_dataF[index + 2] = l_b;

		l_r = Float::Round(l_r);
		l_g = Float::Round(l_g);
		l_b = Float::Round(l_b);

		m_data[index + 0] = (uint8_t)l_r.ToUInt();
		m_data[index + 1] = (uint8_t)l_g.ToUInt();
		m_data[index + 2] = (uint8_t)l_b.ToUInt();
	}
	else {
		m_dataF[index] = (r * 0.299) + (g * 0.587) + (b * 0.114);
		m_dataF[index] = Float::Clamp(m_dataF[index], 0, 255);

		m_data[index] = (uint8_t)Float::Round(m_dataF[index]).ToUInt();
	}
}

void Image::Dither(const int& factor) {
	for (size_t i = 0; i < m_size; i++) {
		int x, y;
		IndexToXY(i, x, y);

		Float threshold = (int)Image::Threshold[(x % 16) + (y % 16) * 16] / Float(256);
		Float octet = 1 / Float(factor);

		Float v = m_dataF[i] / 255;
		v = v + octet * (threshold - 0.5);
		v = Float::Round(v * factor) / factor;
		v = Float::Clamp(v, 0, 1);
		v *= 255;

		m_dataF[i] = v;
		m_data[i] = (uint8_t)Float::Round(v).ToUInt();
	}
}

Float Image::LinearToSRGB(const Float& color) {
#define METHOD1

	Float l_color = color / 255;
	if (color <= 0.0031308) {
		return (color * 12.92) * 255;
	}
	else {
#ifdef METHOD1
		Float out = Float(1) / 2.4;

		out = Float::Pow(l_color, out);
		out *= 1.055;
		out -= 0.055;
		out *= 255;

		return out;
#else
		return ((Float::Pow(color, Float(1) / 2.4) * 1.055) - 0.055) * 255;

#endif // _DEBUG
	}
}

Float Image::sRGBToLinear(const Float& color) {
	Float l_color = color / 255;
	Float out = 0;

	if (l_color <= 0.04045) {
		out = l_color / 12.92;
	}
	else {
		out = Float::Pow((l_color + 0.055) / 1.055, 2.4);
	}

	return out * 255;
}

Image::FileType Image::GetFileType(const char* file) {
	std::string l_file = file;
	std::string type = "";

	bool firstOut = true;
	for (auto c : l_file) {
		if (firstOut) {
			if (c == '.') firstOut = false;
		}
		else {
			type += c;
		}
	}

	if (type.length() != 0) {
		if (type == "bmp") {
			return Image::FileType::BMP;
		}
		else if (type == "jpg") {
			return Image::FileType::JPG;
		}
		else if (type == "png") {
			return Image::FileType::PNG;
		}
		else if (type == "tga") {
			return Image::FileType::TGA;
		}
		else {
			return Image::FileType::NONE;
		}
	}
	return Image::FileType::NONE;
}

int Image::GetIndex(const int& x, const int& y) {
	int newX = x;
	int newY = y;

	if (m_extrapolation == Extrapolation::Repeat) {
		newX = x % m_w;
		newY = y % m_h;

		if (newX < 0) newX = newX + m_w;
		if (newY < 0) newY = newY + m_h;
	}

	newX = std::clamp(newX, 0, m_w - 1);
	newY = std::clamp(newY, 0, m_h - 1);

	return (newX + newY * m_w) * m_channels;
}

void Image::MissingTexture() {
	// create missing texture image

	m_interpolation = Interpolation::Closest;
	m_extrapolation = Extrapolation::Repeat;
	m_colorSpace = ColorSpace::Non_Color;

	m_w = 32;
	m_h = 32;
	m_channels = 3;

	m_size = (size_t)m_w * m_h * m_channels;

	m_data = new uint8_t[m_size];
	m_dataF = new Float[m_size];

	for (size_t i = 0; i < m_size; i++) {
		m_data[i] = 0;
		m_dataF[i] = 0;
	}

	//Float max(255);
	//Float min(0);

	for (int x = 0; x < m_w; x++) {
		for (int y = 0; y < m_h; y++) {
			if ((x < 16 && y < 16) || (x >= 16 && y >= 16)) {
				SetColor(x, y, 255, 0, 255);
			}
			else {
				SetColor(x, y, 0, 0, 0);
			}
		}
	}
}

void Image::FromColorSpace() {
	if (m_colorSpace == ColorSpace::sRGB) {
		for (int x = 0; x < m_w; x++) {
			for (int y = 0; y < m_h; y++) {
				int index = GetIndex(x, y);
				if (m_channels >= 3) {
					for (int i = 0; i < 3; i++) {
						m_dataF[index + i] = Image::sRGBToLinear(m_dataF[index + i]);
						m_dataF[index + i] = Float::Clamp(m_dataF[index + i], 0, 255);

						m_data[index + i] = (uint8_t)Float::Round(m_dataF[index + i]).ToUInt();
					}
				}
				else {
					m_dataF[index] = Image::sRGBToLinear(m_dataF[index]);
					m_dataF[index] = Float::Clamp(m_dataF[index], 0, 255);

					m_data[index] = (uint8_t)Float::Round(m_dataF[index]).ToUInt();
				}
			}
		}
	}
}

void Image::ToColorSpace() {
	if (m_colorSpace == ColorSpace::sRGB) {
		for (int x = 0; x < m_w; x++) {
			for (int y = 0; y < m_h; y++) {
				int index = GetIndex(x, y);
				if (m_channels >= 3) {
					for (int i = 0; i < 3; i++) {
						m_dataF[index + i] = Image::LinearToSRGB(m_dataF[index + i]);
						m_dataF[index + i] = Float::Clamp(m_dataF[index + i], 0, 255);

						m_data[index + i] = (uint8_t)Float::Round(m_dataF[index + i]).ToUInt();
					}
				}
				else {
					m_dataF[index] = Image::LinearToSRGB(m_dataF[index]);
					m_dataF[index] = Float::Clamp(m_dataF[index], 0, 255);

					m_data[index] = (uint8_t)Float::Round(m_dataF[index]).ToUInt();
				}
			}
		}
	}
}

void Image::Bicubic(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	int count = 0;
	if (m_channels >= 3) {
		// image is RGB
		count = 3;
	}
	else {
		// image is grayscale
		count = 1;
	}

	int x1 = Float::Floor(x).ToInt();
	int y1 = Float::Floor(y).ToInt();
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	int x0 = x1 - 1;
	int x3 = x2 + 1;
	int y0 = y1 - 1;
	int y3 = y2 + 1;

	int index00 = GetIndex(x0, y0);
	int index01 = GetIndex(x0, y1);
	int index02 = GetIndex(x0, y2);
	int index03 = GetIndex(x0, y3);

	int index10 = GetIndex(x1, y0);
	int index11 = GetIndex(x1, y1);
	int index12 = GetIndex(x1, y2);
	int index13 = GetIndex(x1, y3);

	int index20 = GetIndex(x2, y0);
	int index21 = GetIndex(x2, y1);
	int index22 = GetIndex(x2, y2);
	int index23 = GetIndex(x2, y3);

	int index30 = GetIndex(x3, y0);
	int index31 = GetIndex(x3, y1);
	int index32 = GetIndex(x3, y2);
	int index33 = GetIndex(x3, y3);

	Float xFactor = x - x1;
	Float yFactor = y - y1;

	for (int i = 0; i < count; i++) {
		Float q00 = m_dataF[index00 + i];
		Float q01 = m_dataF[index01 + i];
		Float q02 = m_dataF[index02 + i];
		Float q03 = m_dataF[index03 + i];

		Float q10 = m_dataF[index10 + i];
		Float q11 = m_dataF[index11 + i];
		Float q12 = m_dataF[index12 + i];
		Float q13 = m_dataF[index13 + i];

		Float q20 = m_dataF[index20 + i];
		Float q21 = m_dataF[index21 + i];
		Float q22 = m_dataF[index22 + i];
		Float q23 = m_dataF[index23 + i];

		Float q30 = m_dataF[index30 + i];
		Float q31 = m_dataF[index31 + i];
		Float q32 = m_dataF[index32 + i];
		Float q33 = m_dataF[index33 + i];

		Float r0 = Float::Cubic(q00, q01, q02, q03, yFactor);
		Float r1 = Float::Cubic(q10, q11, q12, q13, yFactor);
		Float r2 = Float::Cubic(q20, q21, q22, q23, yFactor);
		Float r3 = Float::Cubic(q30, q31, q32, q33, yFactor);

		Float val = Float::Cubic(r0, r1, r2, r3, xFactor);

		if (m_channels >= 3) {
			if (i == 0) {
				r = val;
			}
			else if (i == 1) {
				g = val;
			}
			else {
				b = val;
			}
		}
		else {
			r = val;
			g = val;
			b = val;
		}
	}
}

void Image::Bilinear(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	int count = 0;
	if (m_channels >= 3) {
		// image is RGB
		count = 3;
	}
	else {
		// image is grayscale
		count = 1;
	}

	int lowerX = Float::Floor(x).ToInt();
	int lowerY = Float::Floor(y).ToInt();
	int upperX = Float::Ceil(x).ToInt();
	int upperY = Float::Ceil(y).ToInt();

	int index11 = GetIndex(lowerX, lowerY);
	int index12 = GetIndex(lowerX, upperY);
	int index21 = GetIndex(upperX, lowerY);
	int index22 = GetIndex(upperX, upperY);

	for (int i = 0; i < count; i++) {
		Float q11 = m_dataF[index11 + i];

		Float q12 = m_dataF[index12 + i];

		Float q21 = m_dataF[index21 + i];

		Float q22 = m_dataF[index22 + i];

		Float r1 = Float::Lerp(q11, q21, x - lowerX);
		Float r2 = Float::Lerp(q12, q22, x - lowerX);
		Float temp = Float::Lerp(r1, r2, y - lowerY);

		if (m_channels >= 3) {
			if (i == 0) {
				r = temp;
			}
			else if (i == 1) {
				g = temp;
			}
			else {
				b = temp;
			}
		}
		else {
			r = temp;
			g = temp;
			b = temp;
		}
	}
}

void Image::NearestNeighbour(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	int l_x = Float::Round(x).ToInt();
	int l_y = Float::Round(y).ToInt();

	int index = GetIndex(l_x, l_y);

	if (m_channels >= 3) {
		r = m_dataF[index + 0];
		g = m_dataF[index + 1];
		b = m_dataF[index + 2];
	}
	else {
		r = m_dataF[index];
		g = m_dataF[index];
		b = m_dataF[index];
	}
}

void Image::IndexToXY(const size_t& index, int& x, int& y) {
	int pixelIndex = (int)index / m_channels;
	x = pixelIndex % m_w;
	y = pixelIndex / m_w;
}