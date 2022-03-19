#include <algorithm>
#include <cmath>

#include "FastWrite.h"
#include "String.h"

#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

Image::Image() : Image(1, 1, 1) {
}

Image::Image(const char* file, Image::ColorMode colorMode) {
	Read(file, colorMode);
}

Image::Image(const Image& copyImage) {
	stbi_image_free(m_data);
	delete m_dataF;
	m_dataF = nullptr;

	m_w = copyImage.m_w;
	m_h = copyImage.m_h;
	m_channels = copyImage.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new uint8_t[m_size];
	m_dataF = new float[m_size];

	memcpy(m_data, copyImage.m_data, m_size);
	memcpy(m_dataF, copyImage.m_dataF, m_size);
}

Image::Image(const int w, const int h, const int channels) {
	m_w = w;
	m_h = h;
	m_channels = channels;

	m_size = (size_t)m_w * m_h * m_channels;

	m_data = new uint8_t[m_size];
	m_dataF = new float[m_size];

	for (size_t i = 0; i < m_size; i++) {
		m_data[i] = 0;
		m_dataF[i] = 0.f;
	}
}

Image::~Image() {
	stbi_image_free(m_data);
	delete m_dataF;
	m_dataF = nullptr;
}

Image& Image::operator=(const Image& copyImage) {
	if (&copyImage == this) return *this;

	stbi_image_free(m_data);
	delete m_dataF;
	m_dataF = nullptr;

	m_w = copyImage.m_w;
	m_h = copyImage.m_h;
	m_channels = copyImage.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new uint8_t[m_size];
	m_dataF = new float[m_size];

	memcpy(m_data, copyImage.m_data, m_size);
	memcpy(m_dataF, copyImage.m_dataF, m_size);

	return *this;
}

bool Image::Read(const char* file, Image::ColorMode colorMode) {
	m_data = stbi_load(file, &m_w, &m_h, &m_channels, 0);

	if (m_data == NULL) {
		//std::cout << "\nError reading file: " << file << '\n';
		String output = "";
		output += oof::clear_screen();
		output += oof::position(0, 0);
		output += "\nError reading file: ";
		output += file;
		output += '\n';

		FastWrite::Write(output);

		// create missing texture texture
		m_w = 32;
		m_h = 32;
		m_channels = 3;

		m_size = (size_t)m_w * m_h * m_channels;

		m_data = new uint8_t[m_size];
		m_dataF = new float[m_size];

		for (size_t i = 0; i < m_size; i++) {
			m_data[i] = 0;
			m_dataF[i] = 0.f;
		}

		for (int x = 0; x < m_w; x++) {
			for (int y = 0; y < m_h; y++) {
				if ((x < 16 && y < 16) || (x >= 16 && y >= 16)) {
					SetRGB(x, y, 255.f, 0.f, 255.f);
				}
				else {
					SetRGB(x, y, 0.f, 0.f, 0.f);
				}
			}
		}

		system("pause");
	}
	else {
		m_size = (size_t)m_w * m_h * m_channels;

		m_dataF = new float[m_size];
		for (size_t i = 0; i < m_size; i++) {
			m_dataF[i] = (float)m_data[i];
		}

		if (colorMode == Image::ColorMode::sRGB) {
			ToLinearRGB();
		}
	}

	return m_data != NULL;
}

bool Image::Write(const char* file, Image::ColorMode colorMode) {
	Image::FileType type = GetFileType(file);
	int success = 0;

	if (colorMode == Image::ColorMode::sRGB) {
		TosRGB();
	}

	for (size_t i = 0; i < m_size; i++) {
		m_dataF[i] = m_dataF[i] > 255.f ? 255.f : m_dataF[i];
		m_dataF[i] = m_dataF[i] < 0.f ? 0.f : m_dataF[i];

		m_data[i] = (uint8_t)round(m_dataF[i]);
	}

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
	}

	if (success == 0 && Image::PrintToConsole) {
		std::cout << "\nError writing file: " << file << '\n';
		system("pause");
	}

	return success != 0;
}

void Image::BiLerp(const float x, const float y, float& r, float& g, float& b) {
	int count = 0;

	if (m_channels <= 2) {
		// image is gray scale
		count = 1;
	}
	else {
		// image is RGB
		count = 3;
	}

	for (int i = 0; i < count; i++) {
		int index = 0;

		index = GetIndex((int)floor(x), (int)floor(y));
		float Q11 = m_dataF[index + i];

		index = GetIndex((int)floor(x), (int)ceil(y));
		float Q12 = m_dataF[index + i];

		index = GetIndex((int)ceil(x), (int)floor(y));
		float Q21 = m_dataF[index + i];

		index = GetIndex((int)ceil(x), (int)ceil(y));
		float Q22 = m_dataF[index + i];

		float R1 = std::lerp(Q11, Q21, x - floor(x));
		float R2 = std::lerp(Q12, Q22, x - floor(x));
		float temp = std::lerp(R1, R2, y - floor(y));

		if (m_channels <= 2) {
			r = temp;
			g = temp;
			b = temp;
		}
		else {
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
	}
}

void Image::TosRGB() {
	for (size_t i = 0; i < m_size; i++) {
		float val = m_dataF[i] / 255.f;

		if (val <= 0.0031308f) {
			val *= 12.92f;
		}
		else {
			val = (1.055f * pow(val, 1.f / 2.4f)) - 0.055f;
		}

		val = std::clamp(val, 0.f, 1.f);

		m_dataF[i] = val * 255.f;
		m_data[i] = (unsigned int)round(m_dataF[i]);
	}
}

void Image::ToLinearRGB() {
	for (size_t i = 0; i < m_size; i++) {
		float val = m_dataF[i] / 255.f;

		if (val <= 0.04045f) {
			val /= 12.92f;
		}
		else {
			val = pow((val + 0.055f) / 1.055f, 2.4f);
		}

		val = std::clamp(val, 0.f, 1.f);

		m_dataF[i] = val * 255.f;
		m_data[i] = (unsigned int)round(m_dataF[i]);
	}
}

int Image::GetIndex(const int x, const int y) {
	int clampX;
	int clampY;
	ModulusUV(x, y, clampX, clampY);
	clampX = std::clamp(clampX, 0, m_w - 1);
	clampY = std::clamp(clampY, 0, m_h - 1);
	return (clampX + clampY * m_w) * m_channels;
}

void Image::GetRGB(const int x, const int y, float& r, float& g, float& b) {
	if (m_channels >= 3) {
		int index = GetIndex(x, y);

		r = m_dataF[index + 0];
		g = m_dataF[index + 1];
		b = m_dataF[index + 2];
	}
	else {
		int index = GetIndex(x, y);
		r = m_dataF[index];
		g = m_dataF[index];
		b = m_dataF[index];
	}
}

void Image::SetRGB(const int x, const int y, const float r, const float g, const float b) {
	if (m_channels >= 3) {
		int index = GetIndex(x, y);

		const float l_r = std::clamp(r, 0.f, 255.f);
		const float l_g = std::clamp(g, 0.f, 255.f);
		const float l_b = std::clamp(b, 0.f, 255.f);

		m_dataF[index + 0] = l_r;
		m_dataF[index + 1] = l_g;
		m_dataF[index + 2] = l_b;

		m_data[index + 0] = (uint8_t)round(l_r);
		m_data[index + 1] = (uint8_t)round(l_g);
		m_data[index + 2] = (uint8_t)round(l_b);
	}
	else {
		int index = GetIndex(x, y);

		m_dataF[index] = (0.299f * r) + (0.587f * g) + (0.114f * b);
		m_dataF[index] = std::clamp(m_dataF[index], 0.f, 255.f);

		m_data[index] = (uint8_t)round(m_dataF[index]);
	}
}

Image::FileType Image::GetFileType(const char* file) {
	String l_file = file;
	String type = l_file.GetSecond('.');

	if (type.Length() != 0) {
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
	}
	return Image::FileType::PNG;
}

void Image::ModulusUV(const int x, const int y, int& u, int& v) {
	u = x % m_w;
	v = y % m_h;

	if (u < 0) {
		u = u + m_w;
	}

	if (v < 0) {
		v = v + m_h;
	}
}