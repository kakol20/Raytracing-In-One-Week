#include <algorithm>
#include <cmath>

#include "ImageWrapper.h"

rt::Colour rt::Image::GetPixel(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation, const rt::Image::Interpolation& interpolation, const rt::Image::ColorSpace& colorSpace) {
	sf::Vector2u size = image.getSize();
	if (extrapolation == Extrapolation::Clip && (x < 0 || y < 0 || x >= static_cast<float>(size.x) || y >= static_cast<float>(size.y))) {
		return rt::Colour(0.f, 0.f, 0.f);
	}
	else {
		float l_x = extrapolation == Extrapolation::Extend ? std::clamp(x, 0.f, static_cast<float>(size.x)) : x;
		float l_y = extrapolation == Extrapolation::Extend ? std::clamp(y, 0.f, static_cast<float>(size.y)) : y;

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
			sf::Color out = image.getPixel(static_cast<unsigned int>(std::roundf(l_x)), static_cast<unsigned int>(std::roundf(l_y)));
			return rt::Colour(out.r, out.g, out.b);
		}
	}
}

rt::Colour rt::Image::Bicubic(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation) {
	int x1 = static_cast<int>(std::floorf(x));
	int y1 = static_cast<int>(std::floorf(y));
	int x2 = x1 + 1;
	int y2 = y1 + 1;

	int x0 = x1 - 1;
	int x3 = x2 + 1;
	int y0 = y1 - 1;
	int y3 = y2 + 1;

	// check extrapolation
	sf::Vector2u size = image.getSize();
	if (extrapolation == rt::Image::Extrapolation::Repeat) {
		x0 = x0 % static_cast<int>(size.x);
		x1 = x1 % static_cast<int>(size.x);
		x2 = x2 % static_cast<int>(size.x);
		x3 = x3 % static_cast<int>(size.x);

		y0 = y0 % static_cast<int>(size.y);
		y1 = y1 % static_cast<int>(size.y);
		y2 = y2 % static_cast<int>(size.y);
		y3 = y3 % static_cast<int>(size.y);
	}
	else {
		x0 = std::clamp(x0, 0, static_cast<int>(size.x));
		x1 = std::clamp(x1, 0, static_cast<int>(size.x));
		x2 = std::clamp(x2, 0, static_cast<int>(size.x));
		x3 = std::clamp(x3, 0, static_cast<int>(size.x));

		y0 = std::clamp(y0, 0, static_cast<int>(size.y));
		y1 = std::clamp(y1, 0, static_cast<int>(size.y));
		y2 = std::clamp(y2, 0, static_cast<int>(size.y));
		y3 = std::clamp(y3, 0, static_cast<int>(size.y));
	}

	float xFactor = x - static_cast<float>(x1);
	float yFactor = y - static_cast<float>(y1);

	rt::Colour q00 = image.getPixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y0));
	rt::Colour q01 = image.getPixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y1));
	rt::Colour q02 = image.getPixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y2));
	rt::Colour q03 = image.getPixel(static_cast<unsigned int>(x0), static_cast<unsigned int>(y3));


	return rt::Colour();
}

rt::Colour rt::Image::Bilinear(const float& x, const float& y, const sf::Image& image, const rt::Image::Extrapolation& extrapolation) {
	
	return rt::Colour();
}

//rt::Colour rt::Image::Bicubic(const float& x, const float& y, const sf::Image& image) {
//	unsigned int x1 = static_cast<unsigned int>(std::floorf(x));
//	unsigned int y1 = static_cast<unsigned int>(std::floorf(y));
//	unsigned int x2 = x1 + 1;
//	unsigned int y2 = y1 + 1;
//
//	unsigned int x0 = ;
//	unsigned int x3 = x2 + 1;
//	unsigned int y0 = y1 - 1;
//	unsigned int y3 = y2 + 1;
//
//	return rt::Colour();
//}
//
//rt::Colour rt::Image::Bilinear(const float& x, const float& y, const sf::Image& image) {
//	return rt::Colour();
//}