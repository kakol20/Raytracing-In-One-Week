#include <algorithm>
#include <cmath>

#include "ImageWrapper.h"

rt::Colour rt::Image::GetPixel(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation, const rt::Image::Interpolation& interpolation, const rt::Image::ColorSpace& colorSpace) {
	sf::Vector2u size = image.getSize();
	if (extrapolation == Extrapolation::Clip && (x < 0 || y < 0 || x >= (float)(size.x) || y >= (float)(size.y))) {
		return rt::Colour(0.f, 0.f, 0.f);
	}
	else {
		float l_x = extrapolation == Extrapolation::Extend ? std::clamp(x, 0.f, (float)(size.x)) : x;
		float l_y = extrapolation == Extrapolation::Extend ? std::clamp(y, 0.f, (float)(size.y)) : y;

		if (interpolation == Interpolation::Linear) {
			// bilinear interpolation
			return Bilinear(l_x, l_y, image, extrapolation);
		}
		else if (interpolation == Interpolation::Cubic) {
			// bicubic interpolation
			return Bicubic(l_x, l_y, image, extrapolation);
		}
		else {
			// nearest neighbour
			sf::Color out = image.getPixel((unsigned int)(std::roundf(l_x)), (unsigned int)(std::roundf(l_y)));
			return rt::Colour(out.r, out.g, out.b);
		}
	}
}

rt::Colour rt::Image::Bicubic(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation) {
	int x1 = (int)(std::floorf(x));
	int y1 = (int)(std::floorf(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	int x0 = x1 - 1;
	int x3 = x2 + 1;
	int y0 = y1 - 1;
	int y3 = y2 + 1;

	// check extrapolation
	sf::Vector2u size = image.getSize();
	if (extrapolation == rt::Image::Extrapolation::Repeat) {
		x0 = x0 % (int)(size.x);
		x1 = x1 % (int)(size.x);
		x2 = x2 % (int)(size.x);
		x3 = x3 % (int)(size.x);

		y0 = y0 % (int)(size.y);
		y1 = y1 % (int)(size.y);
		y2 = y2 % (int)(size.y);
		y3 = y3 % (int)(size.y);

		x0 = x0 < 0 ? x0 + (int)size.x : x0;
		x1 = x1 < 0 ? x1 + (int)size.x : x1;
		x2 = x2 < 0 ? x2 + (int)size.x : x2;
		x3 = x3 < 0 ? x3 + (int)size.x : x3;

		y0 = y0 < 0 ? y0 + (int)size.y : y0;
		y1 = y1 < 0 ? y1 + (int)size.y : y1;
		y2 = y2 < 0 ? y2 + (int)size.y : y2;
		y3 = y3 < 0 ? y3 + (int)size.y : y3;
	}
	else {
		x0 = std::clamp(x0, 0, (int)(size.x));
		x1 = std::clamp(x1, 0, (int)(size.x));
		x2 = std::clamp(x2, 0, (int)(size.x));
		x3 = std::clamp(x3, 0, (int)(size.x));

		y0 = std::clamp(y0, 0, (int)(size.y));
		y1 = std::clamp(y1, 0, (int)(size.y));
		y2 = std::clamp(y2, 0, (int)(size.y));
		y3 = std::clamp(y3, 0, (int)(size.y));
	}

	float xFactor = x - (float)(x1);
	float yFactor = y - (float)(y1);

	rt::Colour q00 = image.getPixel((unsigned int)(x0), (unsigned int)(y0));
	rt::Colour q01 = image.getPixel((unsigned int)(x0), (unsigned int)(y1));
	rt::Colour q02 = image.getPixel((unsigned int)(x0), (unsigned int)(y2));
	rt::Colour q03 = image.getPixel((unsigned int)(x0), (unsigned int)(y3));
	rt::Colour r0 = rt::Colour::Cubic(q00, q01, q02, q03, yFactor);

	rt::Colour q10 = image.getPixel((unsigned int)(x1), (unsigned int)(y0));
	rt::Colour q11 = image.getPixel((unsigned int)(x1), (unsigned int)(y1));
	rt::Colour q12 = image.getPixel((unsigned int)(x1), (unsigned int)(y2));
	rt::Colour q13 = image.getPixel((unsigned int)(x1), (unsigned int)(y3));
	rt::Colour r1 = rt::Colour::Cubic(q10, q11, q12, q13, yFactor);

	rt::Colour q20 = image.getPixel((unsigned int)(x2), (unsigned int)(y0));
	rt::Colour q21 = image.getPixel((unsigned int)(x2), (unsigned int)(y1));
	rt::Colour q22 = image.getPixel((unsigned int)(x2), (unsigned int)(y2));
	rt::Colour q23 = image.getPixel((unsigned int)(x2), (unsigned int)(y3));
	rt::Colour r2 = rt::Colour::Cubic(q20, q21, q22, q23, yFactor);

	rt::Colour q30 = image.getPixel((unsigned int)(x3), (unsigned int)(y0));
	rt::Colour q31 = image.getPixel((unsigned int)(x3), (unsigned int)(y1));
	rt::Colour q32 = image.getPixel((unsigned int)(x3), (unsigned int)(y2));
	rt::Colour q33 = image.getPixel((unsigned int)(x3), (unsigned int)(y3));
	rt::Colour r3 = rt::Colour::Cubic(q30, q31, q32, q33, yFactor);

	return rt::Colour::Cubic(r0, r1, r2, r0, xFactor);
}

rt::Colour rt::Image::Bilinear(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation) {
	int x0 = (int)(std::floorf(x));
	int y0 = (int)(std::floorf(y));
	int x1 = x0 + 1;
	int y1 = y0 + 1;

	// check extrapolation
	sf::Vector2u size = image.getSize();
	if (extrapolation == rt::Image::Extrapolation::Repeat) {
		x0 = x0 % (int)(size.x);
		x1 = x1 % (int)(size.x);

		y0 = y0 % (int)(size.y);
		y1 = y1 % (int)(size.y);

		x0 = x0 < 0 ? x0 + (int)size.x : x0;
		x1 = x1 < 0 ? x1 + (int)size.x : x1;

		y0 = y0 < 0 ? y0 + (int)size.y : y0;
		y1 = y1 < 0 ? y1 + (int)size.y : y1;
	}
	else {
		x0 = std::clamp(x0, 0, (int)(size.x));
		x1 = std::clamp(x1, 0, (int)(size.x));

		y0 = std::clamp(y0, 0, (int)(size.y));
		y1 = std::clamp(y1, 0, (int)(size.y));
	}

	float xFactor = x - (float)(x0);
	float yFactor = y - (float)(y0);

	rt::Colour q00 = image.getPixel((unsigned int)(x0), (unsigned int)(y0));
	rt::Colour q01 = image.getPixel((unsigned int)(x0), (unsigned int)(y1));
	rt::Colour r0 = rt::Colour::Linear(q00, q01, yFactor);

	rt::Colour q10 = image.getPixel((unsigned int)(x1), (unsigned int)(y0));
	rt::Colour q11 = image.getPixel((unsigned int)(x1), (unsigned int)(y1));
	rt::Colour r1 = rt::Colour::Linear(q10, q11, yFactor);
	
	return rt::Colour::Linear(r0, r1, xFactor);
}