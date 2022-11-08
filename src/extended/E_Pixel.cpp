#include "E_Pixel.h"

E_Pixel::E_Pixel() {
	m_val = { 0.f, 0.f, 0.f };
}

E_Pixel::E_Pixel(const olc::Pixel& pixel) {
	m_val = { static_cast<float>(pixel.r), static_cast<float>(pixel.g), static_cast<float>(pixel.b) };
}
