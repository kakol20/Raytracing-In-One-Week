#include "HDR.h"

#include "../../ext/stb/stb_image.h"

#include "Image.h"

HDR::HDR(const char* file, const Interpolation interpolation, const Extrapolation extrapolation, const ColorSpace colorSpace) {
	m_interpolation = interpolation;
	m_extrapolation = extrapolation;
	m_colorSpace = colorSpace;

	if (!Read(file)) {
		MissingTexture();
	}
}

HDR::HDR(const HDR& copyHDR) {
	stbi_image_free(m_data);

	m_w = copyHDR.m_w;
	m_h = copyHDR.m_h;
	m_channels = copyHDR.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new float[m_size];

	memcpy(m_data, copyHDR.m_data, m_size);

	m_interpolation = copyHDR.m_interpolation;
	m_extrapolation = copyHDR.m_extrapolation;
	m_colorSpace = copyHDR.m_colorSpace;
}

HDR::~HDR() {
	stbi_image_free(m_data);
}

HDR& HDR::operator=(const HDR& copyHDR) {
	if (this == &copyHDR) return *this;

	stbi_image_free(m_data);

	m_w = copyHDR.m_w;
	m_h = copyHDR.m_h;
	m_channels = copyHDR.m_channels;

	m_size = (size_t)m_w * m_h * m_channels;
	m_data = new float[m_size];

	memcpy(m_data, copyHDR.m_data, m_size);

	m_interpolation = copyHDR.m_interpolation;
	m_extrapolation = copyHDR.m_extrapolation;
	m_colorSpace = copyHDR.m_colorSpace;

	return *this;
}

void HDR::GetColor(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	if (m_extrapolation == Extrapolation::Clip && (x < 0 || y < 0 || x >= m_w || y >= m_h)) {
		r = 0;
		g = 0;
		b = 0;
	}
	else {
		Float l_x = m_extrapolation == Extrapolation::Extend ? Float::Clamp(x, 0, m_w) : x;
		Float l_y = m_extrapolation == Extrapolation::Extend ? Float::Clamp(y, 0, m_h) : y;

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

bool HDR::Read(const char* file) {
	stbi_hdr_to_ldr_gamma(2.2f);
	stbi_hdr_to_ldr_scale(1.0f);

	m_data = stbi_loadf(file, &m_w, &m_h, &m_channels, 0);

	uint8_t* dataI = stbi_load(file, &m_w, &m_h, &m_channels, 0);

	if (m_data == NULL) return false;

	m_size = (size_t)m_w * m_h * m_channels;

	for (size_t i = 0; i < m_size; i++) {
		float val = m_data[i];
		val = val < 0 ? 0 : val;
		val *= 255;

		//val = 0;
		//val += (float)dataI[i];

		if (m_colorSpace == ColorSpace::sRGB) {
			val = (float)Image::sRGBToLinear(val).GetValue();
		}

		m_data[i] = val;
	}

#ifdef _DEBUG
	std::cout << "Read image successful: " << file << '\n';
#endif // _DEBUG

	stbi_image_free(dataI);
	return true;
}

void HDR::SetColor(const int& x, const int& y, const Float& r, const Float& g, const Float& b) {
	int index = GetIndex(x, y);

	if (m_channels >= 3) {
		Float l_r = Float::Clamp(r, 0, 255);
		Float l_g = Float::Clamp(g, 0, 255);
		Float l_b = Float::Clamp(b, 0, 255);

		m_data[index + 0] = (float)l_r.GetValue();
		m_data[index + 1] = (float)l_g.GetValue();
		m_data[index + 2] = (float)l_b.GetValue();
	}
	else {
		Float val = (r * 0.299) + (g * 0.587) + (b * 0.114);

		m_data[index] = (float)val.GetValue();
	}
}

int HDR::GetIndex(const int& x, const int& y) {
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

void HDR::MissingTexture() {
	// create missing texture image

	m_interpolation = Interpolation::Closest;
	m_extrapolation = Extrapolation::Repeat;
	m_colorSpace = ColorSpace::Non_Color;

	m_w = 32;
	m_h = 32;
	m_channels = 3;

	m_size = (size_t)m_w * m_h * m_channels;

	m_data = new float[m_size];

	for (size_t i = 0; i < m_size; i++) {
		m_data[i] = 0;
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

void HDR::Bicubic(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
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
		Float q00 = m_data[index00 + i];
		Float q01 = m_data[index01 + i];
		Float q02 = m_data[index02 + i];
		Float q03 = m_data[index03 + i];

		Float q10 = m_data[index10 + i];
		Float q11 = m_data[index11 + i];
		Float q12 = m_data[index12 + i];
		Float q13 = m_data[index13 + i];

		Float q20 = m_data[index20 + i];
		Float q21 = m_data[index21 + i];
		Float q22 = m_data[index22 + i];
		Float q23 = m_data[index23 + i];

		Float q30 = m_data[index30 + i];
		Float q31 = m_data[index31 + i];
		Float q32 = m_data[index32 + i];
		Float q33 = m_data[index33 + i];

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

void HDR::Bilinear(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
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
		Float q11 = m_data[index11 + i];

		Float q12 = m_data[index12 + i];

		Float q21 = m_data[index21 + i];

		Float q22 = m_data[index22 + i];

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

void HDR::NearestNeighbour(const Float& x, const Float& y, Float& r, Float& g, Float& b) {
	int l_x = Float::Round(x).ToInt();
	int l_y = Float::Round(y).ToInt();

	int index = GetIndex(l_x, l_y);

	if (m_channels >= 3) {
		r = m_data[index + 0];
		g = m_data[index + 1];
		b = m_data[index + 2];
	}
	else {
		r = m_data[index];
		g = m_data[index];
		b = m_data[index];
	}
}