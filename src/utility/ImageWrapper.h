#pragma once

#include <SFML/Main.hpp>
#include <SFML/Graphics.hpp>
	
#include "Colour.h"

namespace rt {
	namespace Image {
		enum class ColorSpace { Non_Color, sRGB };
		enum class Extrapolation { Repeat, Extend, Clip };
		enum class Interpolation { Linear, Closest, Cubic };

		rt::Colour GetPixel(const float& x, const float& y, const sf::Image& image,
			const rt::Image::Extrapolation& extrapolation = rt::Image::Extrapolation::Repeat,
			const rt::Image::Interpolation& interpolation = rt::Image::Interpolation::Linear,
			const rt::Image::ColorSpace& colorSpace = rt::Image::ColorSpace::Non_Color);

		rt::Colour Bicubic(const float& x, const float& y, const sf::Image& image,
			const rt::Image::Extrapolation& extrapolation = rt::Image::Extrapolation::Repeat);
		rt::Colour Bilinear(const float& x, const float& y, const sf::Image& image,
			const rt::Image::Extrapolation& extrapolation = rt::Image::Extrapolation::Repeat);
	};
};