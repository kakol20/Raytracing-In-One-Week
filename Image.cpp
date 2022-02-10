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

	m_size = (size_t)m_w * m_h * m_channels;

	m_dataF = new float[m_size];
	for (size_t i = 0; i < m_size; i++) {
		m_dataF[i] = (float)m_data[i];
	}

	if (m_data == NULL && Image::PrintToConsole) {
		//std::cout << "\nError reading file: " << file << '\n';
		String output = "";
		output += oof::clear_screen();
		output += oof::position(0, 0);
		output += "\nError reading file: ";
		output += file;
		output += '\n';

		FastWrite::Write(output);

		system("pause");
	}

	if (colorMode == Image::ColorMode::sRGB) {
		ToLinearRGB();
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

	return success != 0;;
}

void Image::BiLerp(const float x, const float y, float& r, float& g, float& b) {
	int index = 0;

	index = GetIndex((int)floor(x), (int)floor(y));
	float Q11r = m_dataF[index + 0];
	float Q11g = m_dataF[index + 1];
	float Q11b = m_dataF[index + 2];

	index = GetIndex((int)floor(x), (int)ceil(y));
	float Q12r = m_dataF[index + 0];
	float Q12g = m_dataF[index + 1];
	float Q12b = m_dataF[index + 2];

	index = GetIndex((int)ceil(x), (int)floor(y));
	float Q21r = m_dataF[index + 0];
	float Q21g = m_dataF[index + 1];
	float Q21b = m_dataF[index + 2];

	index = GetIndex((int)ceil(x), (int)ceil(y));
	float Q22r = m_dataF[index + 0];
	float Q22g = m_dataF[index + 1];
	float Q22b = m_dataF[index + 2];

	float R1r = std::lerp(Q11r, Q21r, x - floor(x));
	float R1g = std::lerp(Q11g, Q21g, x - floor(x));
	float R1b = std::lerp(Q11b, Q21b, x - floor(x));

	float R2r = std::lerp(Q12r, Q22r, x - floor(x));
	float R2g = std::lerp(Q12g, Q22g, x - floor(x));
	float R2b = std::lerp(Q12b, Q22b, x - floor(x));

	r = std::lerp(R1r, R2r, y - floor(y));
	g = std::lerp(R1g, R2g, y - floor(y));
	b = std::lerp(R1b, R2b, y - floor(y));
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

		m_data[index + 0] = (unsigned int)round(l_r);
		m_data[index + 1] = (unsigned int)round(l_g);
		m_data[index + 2] = (unsigned int)round(l_b);
	}
}

Image::FileType Image::GetFileType(const char* file) {
	String l_file = file;
	String type = l_file.GetSecond(".");

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
